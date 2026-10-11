/*
 * Copyright (C) 2026 TheCRatTime
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char Peek(const char* text) {
  return (*text == '\0' ? '\0' : *(text+1));
}

static void RealPutChar(const char text) {
  /* Needed escape characters. */
  if (text == '<') {
    fputs("&gt;", stdout);
  } else if (text == '>') {
    fputs("&lt;", stdout);
  } else {
    putchar(text);
  }
}

#define PutChar(chr) \
  RealPutChar(chr)

static void EscapeQuotes(const char text) {
  if (text == '"') {
    fputs("\\\"", stdout);
  } else {
    PutChar(text);
  }
}

static void CodePutChar(const char text) {
  if (text == '\n') {
    fputs("<br>", stdout);
  } else {
    PutChar(text);
  }
}

static const char* HeaderPrint(const char* text) {
  static int was_print = 0;
  int level = 0;
  
  while(*text == '#') {
    level++;
    text++;
  }

  text++;

  if (was_print) {
    fputs("</p>\n", stdout);
  }

  fputs("    <h", stdout);
  putchar(level + '0');
  putchar('>');

  while(*text != '\n' && *text != '\0') {
    PutChar(*(text++));
  }
  
  fputs("</h", stdout);
  putchar(level + '0');
  putchar('>');

  fputs("\n    <p>", stdout);

  was_print = 1;
  
  return text;
}

static const char* PrintColor(const char* text,
                              const char* const first_ptr) {
  int star_count = 0;

  if (text != first_ptr) {
    if (*(text-1) == '\\') {
      return text;
    }
  }

  if (Peek(text) == ' ') {    
    fputs("<br><b>--</b>", stdout);
    return text;
  }
  
  while(*text == '*') {
    star_count++;
    text++;
  }

  switch(star_count) {
    case 1:
      fputs("<i>", stdout);
      break;
    case 2:
    default:
      fputs("<b>", stdout);
      break;
  }

  while(*text != '*' && *text != '\0') {
    PutChar(*(text++));
  }

  while(*text == '*') {
    text++;
  }

  switch(star_count) {
    case 1:
      fputs("</i>", stdout);
      break;
    case 2:
    default:
      fputs("</b>", stdout);
      break;
  }

  return --text;
}

static const char* PrintCode(const char* text,
                             const char* const first_ptr) {
  int quotes_count = 0;
  const char* saved_text = text;
  while(*text == '`') {
    quotes_count++;
    text++;
  }

  if (quotes_count >= 3) {
    
    fputs("<code>", stdout);

    /* Skipping lang name */
    while(*text != '\n' && *text != '\0') {
      text++;
    }

    CodePutChar(*(text++));
    
  redo:
    while(*text != '`' && *text != '\0') {
      CodePutChar(*(text++));
    }

    if (*text == '\0') {
      goto end;
    }

    if (Peek(text) == '\0') {
      goto end;
    }

    if (Peek(text+1) == '\0') {
      goto end;
    }

    if (*text != '`' || Peek(text) != '`' || Peek(text+1) != '`') {
      goto redo;
    }

    text = text+3;
    fputs("</code>", stdout);
  end:;
  }
  else {
    fputs("<code>", stdout);
    
    while(*text != '`' && *text != '\0') {
      PutChar(*(text++));
    }
    
    fputs("</code>", stdout);

    text++;
  }

  text--;

  return text;
}

static const char* PrintBlock(const char* text,
                              const char* const first_ptr) {
  if (text != first_ptr) {
    if (*(text-1) != '\n') {
      fputs("-", stdout);
      return text;
    }
  }

  if (text != first_ptr) {
    if (*(text-1) != '\n') {
      putchar('\n');
    }
  }

  fputs("<br><b>--</b>", stdout);

  return text;
}

static const char* PrintBlock2(const char* text,
                               const char* const first_ptr) {
  if (text != first_ptr) {
    if (*(text-1) != '\n') {
      return text;
    }
  }

  if (Peek(text) == ' ') {
    text++;
  }

  fputs("<br>  ", stdout);
  while(*text != '\n' && *text != '\0') {
    PutChar(*(text++));
  }

  return text;
}

static const char* NewlineCheck(const char* text) {
  fputs("<br>", stdout);

  return text;
}

static const char* ParseTH(const char* text) {
  int quotes = 0;
  const char* copy_text = text;
  
  if (strncmp(text, "TH ", 3)) {
    return text;
  }

  while(*copy_text != '\n' && *copy_text != '\0') {
    if (*copy_text == '"') {
      quotes++;
    }
    
    copy_text++;
  }

  /* 'TH "NAME"' - 2 quotes. */
  if (quotes != 2) {
    fputs(
          "<!-- md2html - TH Invalid. -->\n"
          "<DOCTYPE html>\n"
          "<html lang=\"ru\">\n"
          "<head>\n"
          "    <title>null</title>\n"
          "    <meta charset=\"UTF-8\">\n"
          "    <meta name=\"viewport\" content=\"width=device-width, "
            "initial-scale=1.0\">\n"
          "</head>\n"
          "<body>\n",
          stdout);
    return text;
  }

  fputs(
        "<DOCTYPE html>\n"
        "<html lang=\"en\">\n"
        "<head>\n"
        "    <title>", stdout);
  text += 4;

  while(*text != '"' && *text != '\n' && *text != '\0') {
    PutChar(*(text++));
  }

  fputs(
        "</title>\n"
        "    <meta charset=\"UTF-8\">\n"
        "    <meta name=\"viewport\" content=\"width=device-width, "
          "initial-scale=1.0\">\n"
        "</head>\n<body>", stdout);
  
  return ++text;
}

static void PutComment(void) {
  fputs(
        "<!-- Automatically generated by md2html -->\n"
        "<!--            Do not edit.            -->\n"
        "<!--    Copyright (C) TheCRatTime =)    -->\n"
        "<!--      Begin generated content:      -->\n",
        stdout);
}

static const char* PrintLink(const char* text) {
  const char* saved_text = ++text;
  size_t link_len = 0;

  fputs("<a href=\"", stdout);
  
  while(*saved_text != ']' && *saved_text != '\0') {
    saved_text++;
  }

  if (*saved_text == '\0') {
  error:
    fputs("/\">bad link</a>", stdout);
    return text;
  }
  
  if (Peek(saved_text) == '\0' || Peek(saved_text+1) == '\0') {
    goto error;
  }
  
  saved_text += 2;

  while(*saved_text != ')' && *saved_text != '\0') {
    link_len++;
    EscapeQuotes(*(saved_text++));
  }

  fputs("\">", stdout);

  while(*text != ']' && *text != '\0') {
    PutChar(*(text++));
  }

  fputs("</a>", stdout);

  text += link_len + 2;
  
  return text;
}

static const char* Backslash(const char* text) {
  if (Peek(text) != '*') {
    PutChar('\\');
    return text;
  }

  PutChar('*');
  
  return text;
}

static void PrintEnd(void) {
  fputs(
        "    </p>\n"
        "</body>\n"
        "</html>\n", stdout);
} 

static void PrettyPrint(const char* text) {
  const char* const first_text = text;
  
  PutComment();
  
  text = ParseTH(text);
  
  while(*text) {
    
    switch(*text) {
      case '#':
        text = HeaderPrint(text);
        break;
      case '*':
        text = PrintColor(text, first_text);
        break;
      case '`':
        text = PrintCode(text, first_text);
        break;
      case '-':
        text = PrintBlock(text, first_text);
        break;
      case '>':
        text = PrintBlock2(text, first_text);
        break;
      case '\n':
        text = NewlineCheck(text);
        break;
      case '[':
        text = PrintLink(text);
        break;
      case '\\':
        text = Backslash(text);
        break;
      default:
        PutChar(*text);
        break;
    }
    
    if (*text != '\0') {
      text++;
    }
  }

  PrintEnd();
}

int main(int argc, char** argv) {
  FILE* file = NULL;
  char* buffer = NULL;
  size_t size = 0;
  
  if (argc < 2) {
    printf("Usage: %s FILE > OUTFILE\n", argv[0]);
    return 1;
  }

  file = fopen(argv[1], "rb");
  if (file == NULL) {
    perror(argv[0]);
    return 1;
  }

  fseek(file, 0, SEEK_END);
  size = (size_t)ftell(file);
  fseek(file, 0, SEEK_SET);

  buffer = (char*)malloc(size+1);

  if (buffer == NULL) {
    perror(argv[0]);
    fclose(file);
    return 1;
  }

  if (fread(buffer, 1, size, file) <= 0) {
    perror(argv[0]);
    free(buffer);
    fclose(file);
    return 1;
  }

  buffer[size] = '\0';
  fclose(file);

  PrettyPrint(buffer);

  free(buffer);
  return 0;
}
