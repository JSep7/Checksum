/*============================================================================
 |   Assignment: pa01 - Calculate the checksum of an input file given:
 |              -> the name of the input file,
 |              -> the checksum size of either 8, 16, or 32 bits
 |   Author: Jaime Sepulveda
 |   Language:   C
 |   To Compile: gcc -o pa01 pa01.c
 |   To Execute: C ->  ./pa01 inputFilename.txt checksumSize
 |                      where inputFilename.txt is the input file
 |                      and checksumSize is either 8, 16, or 32
 |   Note:
 |               All input files are simple 8 bit ASCII input
 |               All execute commands above have been tested on Eustis
 |   Class:      CIS3360 - Security in Computing - Fall 2025
 |   Instructor: McAlpin
 |   Due Date:   09/21/2025
 +===========================================================================*/

#include <stdio.h>
#include <stdlib.h>

// Function that helps print a new line after the column maxes out after 80 chars 
static void word_wrapper(const unsigned char *buf, int len) {
  int col = 0;
  for (int i = 0; i < len; i++) {
    // Sets a char c to access whatever index char is
    unsigned char c = buf[i]; 
    putchar(c);
 
  // If the char c is a newline then we will set the col back to 0 to indicate 80 chars 
  if (c == '\n') {
    col = 0;
  }
  // Increase column count if its not full
  else {
    col++;
    if (col == 80) {
      // Force a newline once it has 80 Chars
      putchar('\n');
      col = 0;
      }
    }
  }
}

// Function that helps build a word by combining the bytes and pads with 'X' when necessary
unsigned long int make_word(const unsigned char *buf, size_t start, int size, size_t word_count) {
    unsigned long int w = 0;
    for (int i = 0; i < size; i++) {
      // Shifts to the left to make room
      w <<= 8;
      if (start + i < word_count) {
	      // Adds byte to the buffer
        w |= (unsigned long int)buf[start + i];
      }
      else {
	      // Pad with 'X'
        w |= (unsigned long int)'X';
      }
    }
  return w;
}

int main(int argc, char *argv[]) {

  // This if statement ensures that there is no more than 3 arguments expected 
  if (argc != 3) {
    fprintf(stderr, "Usage: %s <filename> <checksum size> \n", argv[0]);
    return 1;
  }

  // Filename argument expected
  const char *file = argv[1];
  // Size for the check sum argument expected
  int check_size  = atoi(argv[2]);

  // If the size doesnt fit the correct number of bits right an error message
  if (check_size != 8 && check_size != 16 && check_size != 32) {
    fprintf(stderr, "Valid checksum sizes are 8, 16, or 32\n");
    return 1;
  }

  // Attempts to open the file given and if its not a valid file it will print an error
  FILE *fp = fopen(file, "rb");
  if (!fp) {
    fprintf(stderr, "Error: could not open '%s'\n", file);
    return 1;
  }

  // Pointer to the array in the file contents
  unsigned char *buf = NULL;
  // Current capacity allocated of buf and word_count is our counter from how many bytes we read so far
  size_t buf_cap = 0, word_count = 0;

  // Infinite while loop so that we can keep reading until the end
  while (1) {
    // If there isnt enough space allocate new memory for the byte
    if (word_count + 4096 > buf_cap) {
      size_t new_cap;

      // If empty allocate a byte otherwise double it
      if (buf_cap == 0) {
	      new_cap = 4096;
      }
      else {
	      new_cap = buf_cap * 2;
      }

      // Reallocate the buffer with what has been added  
      unsigned char *temp = realloc(buf, new_cap);
      if (!temp) {
	      fprintf(stderr, "Error: no memory \n");
	      fclose(fp);
	      free(buf);
	      return 1;
      }
      buf = temp;
      buf_cap = new_cap;
    }
    
    // Read the bytes into the buffer
    size_t x = fread(buf + word_count, 1, buf_cap - word_count, fp);
    word_count += x;

    // Stop if there isnt anything left to read
    if (x == 0) {
	    break;
    }
  }

  fclose(fp);
  
  word_wrapper(buf, (int)word_count);

  // Calls the pad functions if the bytes are smaller than 8, 16, and 32
  int group_size = check_size / 8;
  int pad = (group_size - ((int)word_count % group_size)) % group_size;
  for (int i = 0; i < pad; i++) {
    putchar('X');
  }
  putchar('\n');

  // Checksum
  unsigned long int check_sum = 0;

  // If statements that will compute the checksum based on the size
  if (check_size == 8) {
    // Adds the byte to the check sum and also computes any padding
    for (size_t i = 0; i < word_count; i++) {
        check_sum += (unsigned long int)buf[i];
    }

    for (int i = 0; i < pad; i++) {
        check_sum += (unsigned long int)'X';
    }
    // We are setting it to 8 bits
    check_sum = check_sum % 256;
  }
  else if (check_size == 16) {
    // Adds this 16 byte to the check sum and does padding also
    for (size_t i = 0; i < word_count + pad; i += 2) {
      // Calls the make_word function in for the bitmasking and padding
      check_sum += make_word(buf, i, 2, word_count);
    }
    // Sets it to 16 bits
    check_sum = check_sum % 65536;
  }
  else if (check_size == 32) {
    // Adds this 32 byte to the check sum and does padding also
    for (size_t i = 0; i < word_count + pad; i += 4) {
      check_sum += make_word(buf, i, 4, word_count);
    }
    check_sum = check_sum % 4294967296;
  }
  // Adds all the chars and makes sure to include padding
  int count = (int)(word_count + pad);
  
  printf("%2d bit checksum is %8lx for all %4d chars\n", check_size, check_sum, count);

  free(buf);
  return 0;
}

/*=============================================================================                      
 |    I [Jaime Sepulveda] ([5666290]) affirm that this program is                                    
 | entirely my own work and that I have neither developed my code together with                      
 | any another person, nor copied any code from any other person, nor permitted                      
 | my code to be copied or otherwise used by any other person, nor have I                            
 | copied, modified, or otherwise used programs created by others. I acknowledge                     
 | that any violation of the above terms will be treated as academic dishonesty.                     
 +=============================================================================*/
