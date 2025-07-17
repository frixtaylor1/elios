#include <allocators/alloc.h>
#include <string.h>
#include <assert.h>

typedef struct Elios_Packed  MemHeader {
  word words: 30;
  bool reserved: 1;
  bool alloced: 1;
} MemHeader;

Elios_Public extern heap *memspace;

#define $header (MemHeader *)

const int8 WordSize = sizeof(word);

Elios_Private void init_header(MemHeader *header, word words, bool alloced, bool reserved) {
  header->words = words;
  header->alloced = alloced;
  header->reserved = reserved;
}
Elios_Private int8 *lastAddress;

Elios_Private Elios_Constructor void init_module() {
 lastAddress = ($int8 memspace + (1024 * 1024 * 1024));
}

Elios_Private word calculate_words_given_bytes(int32 bytes) {
  return !(bytes % WordSize) ? bytes / WordSize : bytes / WordSize + 1;
}

Elios_Private MemHeader *get_header_given_ptr(void *ptr) { return $header ptr - 1; }

Elios_Private int32 get_bytes_alloc_region_given_header(MemHeader *header) {
  return header->words * WordSize;
}

Elios_Private MemHeader *next_header(MemHeader *header) {
  return $header($int8(header + 1) + header->words * WordSize);
}

Elios_Private MemHeader *find_block(MemHeader *current, word wordsToAlloc) {
  IfTrue ($int8 current >= lastAddress) {
    return 0;
  }
  IfTrue (!current->alloced)
    return current;

  return find_block(next_header(current), wordsToAlloc);
}

Elios_Private void coalesce(MemHeader *header) {
  MemHeader *next = next_header(header);
  IfTrue (next && !next->alloced) {
    header->words += 1 + next->words;
  }
}

Elios_Public void *mem_alloc(int32 bytes) {
  void *mem = memspace;
  MemHeader *header = $header mem;
  word wordsToAlloc = calculate_words_given_bytes(bytes);

  IfTrue (!bytes)
    return 0;

  IfTrue (!header->alloced) {
    IfTrue (header->words == 0) {
      init_header(header, wordsToAlloc, true, false);
      return $header mem + 1;
    }

    IfTrue (header->words >= wordsToAlloc) {
      word difference = header->words - wordsToAlloc;
      init_header(header, wordsToAlloc, true, false);

      MemHeader *next = next_header(header);
      init_header(next, difference, false, false);

      return $void(header + 1);
    }
  }

  MemHeader *selectedHeader = find_block(header, wordsToAlloc);
  IfTrue ((bool) selectedHeader) {
    word difference = selectedHeader->words - wordsToAlloc;

    selectedHeader->words = wordsToAlloc;
    selectedHeader->alloced = true;

    IfTrue (difference > 0) {
      MemHeader *next = next_header(selectedHeader);
      init_header(next, difference, false, false);
    }

    return (void *)(selectedHeader + 1);
  }
  assert(false && "Error no mem!");
}

Elios_Public void mem_dealloc(void *ptr) {
  MemHeader *header = get_header_given_ptr(ptr);

  IfTrue (!header->alloced)
    assert(header->alloced && "Double free! %d");
  header->alloced = 0;

  memset(ptr, 0, get_bytes_alloc_region_given_header(header));

  coalesce(header);
}

Elios_Public void *mem_realloc(void *ptr, int32 bytes) {
  MemHeader *header = get_header_given_ptr(ptr);

  word wordsNeeded = calculate_words_given_bytes(bytes);
  IfTrue (header->words >= wordsNeeded) {
    header->words = wordsNeeded;
    return ptr;
  }

  mem_dealloc(ptr);

  return mem_alloc(bytes);
}