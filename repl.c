#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
typedef struct {
  char* buffer;
  size_t buffer_length;
  // signed version of size_t
  ssize_t input_length;
} InputBuffer;

InputBuffer* initialize_input_buffer() {
  // manually allocate
  InputBuffer* input_buffer = (InputBuffer*)malloc(sizeof(InputBuffer));
  input_buffer->buffer = NULL;
  input_buffer->buffer_length = 0;
  input_buffer->input_length = 0;

  return input_buffer;
}
void print_prompt() {printf(">>> "); }
// this getline is written by claude
ssize_t getline (char** lineptr, size_t* n, FILE* stream)
{
  if (lineptr == NULL || n == NULL || stream == NULL) {
    return -1;
  }

  // allocate a starting buffer if none exists yet
  if (*lineptr == NULL || *n == 0) {
    *n = 128;
    *lineptr = (char*)malloc(*n);
    if (*lineptr == NULL) {
      return -1;
    }
  }

  size_t pos = 0;
  int c;
  while ((c = fgetc(stream)) != EOF) {
    // keep room for the new char plus the '\0'
    if (pos + 2 > *n) {
      size_t new_size = *n * 2;
      char* new_buffer = (char*)realloc(*lineptr, new_size);
      if (new_buffer == NULL) {
        return -1;
      }
      *lineptr = new_buffer;
      *n = new_size;
    }

    (*lineptr)[pos++] = (char)c;
    if (c == '\n') {
      break;
    }
  }

  // nothing was read, so it's EOF or an error
  if (pos == 0) {
    return -1;
  }

  (*lineptr)[pos] = '\0';
  return (ssize_t)pos;
}

void read_input(InputBuffer* input_buffer) {
  ssize_t bytes_read =
      getline(&(input_buffer->buffer), &(input_buffer->buffer_length), stdin);

  if (bytes_read <= 0) {
    printf("Error reading input\n");
    exit(EXIT_FAILURE);
  }

  // Ignore trailing newline
  input_buffer->input_length = bytes_read - 1;
  input_buffer->buffer[bytes_read - 1] = 0;
}

// free memory
void close_input_buffer(InputBuffer* input_buffer) {
  free(input_buffer->buffer);
  free(input_buffer);
}
typedef enum
{
  META_COMMAND_SUCCESS,
  META_COMMAND_UNRECOGNIZED_COMMAND
} MetaCommandResult;

typedef enum
{
  PREPARE_SUCCESS,
  PREPARE_UNRECOGNIZED_STATEMENT
} PrepareResult;

MetaCommandResult do_meta_command(InputBuffer* input_buffer)
{
  if (strcmp(input_buffer->buffer, ".exit") == 0)
  {
    exit(EXIT_SUCCESS);
  }
  else
  {
    return META_COMMAND_UNRECOGNIZED_COMMAND;
  }
}

typedef enum
{
  STATEMENT_INSERT,
  STATEMENT_SELECT
} StatementType;

typedef struct
{
  StatementType type;
} Statement;

PrepareResult prepare_statement(InputBuffer* input_buffer,
                                Statement* statement) {
  if (strncmp(input_buffer->buffer, "insert", 6) == 0)
  {
    statement->type = STATEMENT_INSERT;
    return PREPARE_SUCCESS;
  }
  if (strcmp(input_buffer->buffer, "select") == 0)
  {
    statement->type = STATEMENT_SELECT;
    return PREPARE_SUCCESS;
  }

  return PREPARE_UNRECOGNIZED_STATEMENT;
}

void execute_statement(Statement* statement) {
  switch (statement->type)
  {
    case (STATEMENT_INSERT):
      printf("This is where we would do an insert.\n");
      break;
    case (STATEMENT_SELECT):
      printf("This is where we would do a select.\n");
      break;
  }
}

int main(int argc, char* argv[])
{
  InputBuffer* input_buffer = initialize_input_buffer();
  bool is_running = true;
  while (is_running)
  {
    print_prompt();
    read_input(input_buffer);

    if (strcmp(input_buffer->buffer, ".exit") == 0)
    {
      if (input_buffer->buffer[0] == '.')
      {
        switch (do_meta_command(input_buffer))
        {
          case (META_COMMAND_SUCCESS):
            continue;
          case (META_COMMAND_UNRECOGNIZED_COMMAND):
            printf("Unrecognized command '%s'\n", input_buffer->buffer);
            continue;
        }
      }
    }

    Statement statement;
    switch (prepare_statement(input_buffer, &statement))
    {
      case (PREPARE_SUCCESS):
        break;
      case (PREPARE_UNRECOGNIZED_STATEMENT):
        printf("Unrecognized keyword at start of '%s'.\n",
               input_buffer->buffer);
        continue;
    }

    execute_statement(&statement);
    printf("Executed.\n");
  }
  return 0;
}
