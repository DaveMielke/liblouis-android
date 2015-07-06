#include <yaml.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <error.h>
#include "liblouis.h"
#include "brl_checks.h"

const char* event_names[] = {"YAML_NO_EVENT", "YAML_STREAM_START_EVENT", "YAML_STREAM_END_EVENT",
			     "YAML_DOCUMENT_START_EVENT", "YAML_DOCUMENT_END_EVENT",
			     "YAML_ALIAS_EVENT", "YAML_SCALAR_EVENT",
			     "YAML_SEQUENCE_START_EVENT", "YAML_SEQUENCE_END_EVENT",
			     "YAML_MAPPING_START_EVENT", "YAML_MAPPING_END_EVENT"};
const char* encoding_names[] = {"YAML_ANY_ENCODING", "YAML_UTF8_ENCODING",
				"YAML_UTF16LE_ENCODING", "YAML_UTF16BE_ENCODING"};

yaml_parser_t parser;
yaml_event_t event;

char *file_name;
int translation_mode = 0;

int errors = 0;
int count = 0;

int
simple_error (const char *msg, yaml_event_t *event) {
  error_at_line(EXIT_FAILURE, 0, file_name, event->start_mark.line, msg);
}

int
yaml_error (yaml_event_type_t expected, yaml_event_t *event) {
  error_at_line(EXIT_FAILURE, 0, file_name, event->start_mark.line,
		"Expected %s (actual %s)",
		event_names[expected], event_names[event->type]);
}

void
read_tables (yaml_parser_t *parser, char *tables_list) {
  yaml_event_t event;
  if (!yaml_parser_parse(parser, &event) ||
      (event.type != YAML_SCALAR_EVENT) ||
      strcmp(event.data.scalar.value, "tables")) {
    simple_error("tables expected", &event);
  }

  yaml_event_delete(&event);

  if (!yaml_parser_parse(parser, &event) ||
      (event.type != YAML_SEQUENCE_START_EVENT))
    yaml_error(YAML_SEQUENCE_START_EVENT, &event);

  yaml_event_delete(&event);

  int done = 0;
  char *p = tables_list;
  while (!done) {
    if (!yaml_parser_parse(parser, &event)) {
      simple_error("Error in YAML", &event);
    }
    if (event.type == YAML_SEQUENCE_END_EVENT) {
      done = 1;
    } else if (event.type == YAML_SCALAR_EVENT ) {
      if (tables_list != p) strcat(p++, ",");
      strcat(p, event.data.scalar.value);
      p += event.data.scalar.length;
    }
    yaml_event_delete(&event);
  }
}

void
read_flags (yaml_parser_t *parser) {
  yaml_event_t event;
  if (!yaml_parser_parse(parser, &event) ||
      (event.type != YAML_MAPPING_START_EVENT))
    yaml_error(YAML_MAPPING_START_EVENT, &event);

  yaml_event_delete(&event);

  int done = 0;
  while (!done) {
    if (!yaml_parser_parse(parser, &event)) {
      simple_error("Error in YAML", &event);
    }
    if (event.type == YAML_MAPPING_END_EVENT) {
      done = 1;
    } else if (event.type == YAML_SCALAR_EVENT ) {
      printf("Flag %s\n", event.data.scalar.value);
    }
    yaml_event_delete(&event);
  }
}

int
read_xfail (yaml_parser_t *parser) {
  yaml_event_t event;
  int xfail = 0;
  if (!yaml_parser_parse(parser, &event) ||
      (event.type != YAML_SCALAR_EVENT))
    yaml_error(YAML_SCALAR_EVENT, &event);
  if (!strcmp(event.data.scalar.value, "Y") ||
      !strcmp(event.data.scalar.value, "true") ||
      !strcmp(event.data.scalar.value, "Yes") ||
      !strcmp(event.data.scalar.value, "ON"))
    xfail = 1;
  yaml_event_delete(&event);
  return xfail;
}

translationModes
read_mode (yaml_parser_t *parser) {
  yaml_event_t event;
  translationModes mode = 0;
  int parse_error = 1;

  if (!yaml_parser_parse(parser, &event) ||
      (event.type != YAML_SEQUENCE_START_EVENT))
    yaml_error(YAML_SEQUENCE_START_EVENT, &event);
  yaml_event_delete(&event);

  while ((parse_error = yaml_parser_parse(parser, &event)) &&
	 (event.type == YAML_SCALAR_EVENT)) {
    if (!strcmp(event.data.scalar.value, "noContractions")) {
      mode |= noContractions;
    } else if (!strcmp(event.data.scalar.value, "compbrlAtCursor")) {
      mode |= compbrlAtCursor;
    } else if (!strcmp(event.data.scalar.value, "dotsIO")) {
      mode |= dotsIO;
    } else if (!strcmp(event.data.scalar.value, "comp8Dots")) {
      mode |= comp8Dots;
    } else if (!strcmp(event.data.scalar.value, "pass1Only")) {
      mode |= pass1Only;
    } else if (!strcmp(event.data.scalar.value, "compbrlLeftCursor")) {
      mode |= compbrlLeftCursor;
    } else if (!strcmp(event.data.scalar.value, "otherTrans")) {
      mode |= otherTrans;
    } else if (!strcmp(event.data.scalar.value, "ucBrl")) {
      mode |= ucBrl;
    } else {
      break;
    }
    yaml_event_delete(&event);
  }
  if (!parse_error)
    simple_error("Error in YAML", &event);
  if (event.type != YAML_SEQUENCE_END_EVENT)
    yaml_error(YAML_SEQUENCE_END_EVENT, &event);
  yaml_event_delete(&event);
  return mode;
}

int *
read_cursorPos (yaml_parser_t *parser, int len) {
  int *pos = malloc(sizeof(int) * len);
  int i = 0;
  yaml_event_t event;
  int parse_error = 1;
  char *tail;

  if (!yaml_parser_parse(parser, &event) ||
      (event.type != YAML_SEQUENCE_START_EVENT))
    yaml_error(YAML_SEQUENCE_START_EVENT, &event);
  yaml_event_delete(&event);

  while ((parse_error = yaml_parser_parse(parser, &event)) &&
	 (event.type == YAML_SCALAR_EVENT)) {
    pos[i++] = strtol(event.data.scalar.value, &tail, 0);
    if (i > len)
      error_at_line(EXIT_FAILURE, 0, file_name, event.start_mark.line,
		    "Too many cursor positions (%i) for word of length %i\n", i, len);
    if (!pos && !strcmp(event.data.scalar.value, tail))
      error_at_line(EXIT_FAILURE, 0, file_name, event.start_mark.line,
		    "Not a valid cursor position '%s'. Must be a number\n",
		    event.data.scalar.value);
    yaml_event_delete(&event);
  }
  if (!parse_error)
    simple_error("Error in YAML", &event);
  if (event.type != YAML_SEQUENCE_END_EVENT)
    yaml_error(YAML_SEQUENCE_END_EVENT, &event);
  yaml_event_delete(&event);
  return pos;
}

void
read_options (yaml_parser_t *parser, int len,
	      int *xfail, translationModes *mode,
	      char *typeform, int **cursorPos) {
  yaml_event_t event;
  char *option_name;
  int parse_error = 1;

  *mode = 0;
  *xfail = 0;
  typeform = NULL;
  *cursorPos = NULL;

  while ((parse_error = yaml_parser_parse(parser, &event)) &&
	 (event.type == YAML_SCALAR_EVENT)) {
    option_name = strndup(event.data.scalar.value, event.data.scalar.length);

    if (!strcmp(option_name, "xfail")) {
      yaml_event_delete(&event);
      *xfail = read_xfail(parser);
    } else if (!strcmp(option_name, "mode")) {
      yaml_event_delete(&event);
      *mode = read_mode(parser);
    } else if (!strcmp(option_name, "typeform")) {
      yaml_event_delete(&event);
      if (!yaml_parser_parse(parser, &event) ||
	  (event.type != YAML_SCALAR_EVENT))
	yaml_error(YAML_SCALAR_EVENT, &event);
      typeform = convert_typeform(event.data.scalar.value);
      yaml_event_delete(&event);
    } else if (!strcmp(option_name, "cursorPos")) {
      yaml_event_delete(&event);
      *cursorPos = read_cursorPos(parser, len);
    } else {
      error_at_line(EXIT_FAILURE, 0, file_name, event.start_mark.line,
		    "Unsupported option %s", option_name);
    }
  }
  if (!parse_error)
    simple_error("Error in YAML", &event);
  if (event.type != YAML_MAPPING_END_EVENT)
    yaml_error(YAML_MAPPING_END_EVENT, &event);
  yaml_event_delete(&event);
}

void
read_test(yaml_parser_t *parser, char *tables_list) {
  yaml_event_t event;
  char *word;
  char *translation;
  int xfail = 0;
  translationModes mode = 0;
  char *typeform = NULL;
  int *cursorPos = NULL;

  if (!yaml_parser_parse(parser, &event) ||
      (event.type != YAML_SCALAR_EVENT))
    simple_error("Word expected", &event);

  word = strndup(event.data.scalar.value, event.data.scalar.length);
  yaml_event_delete(&event);

  if (!yaml_parser_parse(parser, &event) ||
      (event.type != YAML_SCALAR_EVENT))
    simple_error("Translation expected", &event);

  translation = strndup(event.data.scalar.value, event.data.scalar.length);
  yaml_event_delete(&event);

  if (!yaml_parser_parse(parser, &event))
    simple_error("Error in YAML", &event);

  if (event.type == YAML_MAPPING_START_EVENT) {
    yaml_event_delete(&event);
    read_options(parser, strlen(word), &xfail, &mode, typeform, &cursorPos);

    if (!yaml_parser_parse(parser, &event) ||
	(event.type != YAML_SEQUENCE_END_EVENT))
      yaml_error(YAML_SEQUENCE_END_EVENT, &event);
  } else if (event.type != YAML_SEQUENCE_END_EVENT) {
    simple_error("Unexpected event", &event);
  }

  if (!cursorPos) {
    if (xfail != check_translation_with_mode(tables_list, word, typeform,
					     translation, translation_mode)) {
      fprintf(stderr, "%s:%zu Test Failure\n", file_name, event.start_mark.line);
      errors++;
    }
  } else {
    if (xfail != check_cursor_pos(tables_list, word, cursorPos)) {
      fprintf(stderr, "%s:%zu Test Failure\n", file_name, event.start_mark.line);
      errors++;
    }
  }
  yaml_event_delete(&event);
  count++;
  free(word);
  free(translation);
  if (typeform)
    free(typeform);
  if (cursorPos)
    free(cursorPos);
}

void
read_tests(yaml_parser_t *parser, char *tables_list) {
  yaml_event_t event;
  if (!yaml_parser_parse(parser, &event) ||
      (event.type != YAML_SEQUENCE_START_EVENT))
    yaml_error(YAML_SEQUENCE_START_EVENT, &event);

  yaml_event_delete(&event);

  int done = 0;
  while (!done) {
    if (!yaml_parser_parse(parser, &event)) {
      simple_error("Error in YAML", &event);
    }
    if (event.type == YAML_SEQUENCE_END_EVENT) {
      done = 1;
      yaml_event_delete(&event);
    } else if (event.type == YAML_SEQUENCE_START_EVENT ) {
      yaml_event_delete(&event);
      read_test(parser, tables_list);
    } else {
      simple_error("Unexpected event", &event);
    }
  }
}

int
main(int argc, char *argv[]) {
  if (argc != 2) {
    printf("Usage: %s file.yaml\n", argv[0]);
    return 0;
  }

  FILE *file;
  yaml_parser_t parser;
  yaml_event_t event;

  file = fopen(argv[1], "rb");
  assert(file);

  file_name = argv[1];

  assert(yaml_parser_initialize(&parser));

  yaml_parser_set_input_file(&parser, file);

  if (!yaml_parser_parse(&parser, &event) ||
      (event.type != YAML_STREAM_START_EVENT)) {
    yaml_error(YAML_STREAM_START_EVENT, &event);
  }

  if (event.data.stream_start.encoding != YAML_UTF8_ENCODING)
    error_at_line(EXIT_FAILURE, 0, file_name, event.start_mark.line,
		  "UTF-8 encoding expected (%s instead)",
		  encoding_names[event.data.stream_start.encoding]);
  yaml_event_delete(&event);

  if (!yaml_parser_parse(&parser, &event) ||
      (event.type != YAML_DOCUMENT_START_EVENT)) {
    yaml_error(YAML_DOCUMENT_START_EVENT, &event);
  }
  yaml_event_delete(&event);

  if (!yaml_parser_parse(&parser, &event) ||
      (event.type != YAML_MAPPING_START_EVENT)) {
    yaml_error(YAML_MAPPING_START_EVENT, &event);
  }
  yaml_event_delete(&event);

  char *tables_list = malloc(sizeof(char) * 512);
  read_tables(&parser, tables_list);

  if (!yaml_parser_parse(&parser, &event) ||
      (event.type != YAML_SCALAR_EVENT)) {
    yaml_error(YAML_SCALAR_EVENT, &event);
  }

  if (!strcmp(event.data.scalar.value, "flags")) {
    yaml_event_delete(&event);
    read_flags(&parser);

    if (!yaml_parser_parse(&parser, &event) ||
	(event.type != YAML_SCALAR_EVENT) ||
	strcmp(event.data.scalar.value, "tests")) {
      simple_error("tests expected", &event);
    }
    yaml_event_delete(&event);
    read_tests(&parser, tables_list);

  } else if (!strcmp(event.data.scalar.value, "tests")) {
    yaml_event_delete(&event);
    read_tests(&parser, tables_list);
  } else {
    simple_error("flags or tests expected", &event);
  }

  if (!yaml_parser_parse(&parser, &event) ||
      (event.type != YAML_MAPPING_END_EVENT)) {
    yaml_error(YAML_MAPPING_END_EVENT, &event);
  }
  yaml_event_delete(&event);

  if (!yaml_parser_parse(&parser, &event) ||
      (event.type != YAML_DOCUMENT_END_EVENT)) {
    yaml_error(YAML_DOCUMENT_END_EVENT, &event);
  }
  yaml_event_delete(&event);

  if (!yaml_parser_parse(&parser, &event) ||
      (event.type != YAML_STREAM_END_EVENT)) {
    yaml_error(YAML_STREAM_END_EVENT, &event);
  }
  yaml_event_delete(&event);

  yaml_parser_delete(&parser);

  assert(!fclose(file));

  printf("%s (%d tests, %d failures)\n", (errors ? "FAILURE" : "SUCCESS"), count, errors);

  return errors ? 1 : 0;
}
