#define arraylen(A) (sizeof(A)/sizeof(A[0]))

/* make strings less of a pain in the ass */
#define stralloc(stringinit) malloc(strlen(stringinit))
char* strgrow(char* str, int grow);
char* strappend(char* str, char* append);
char* strclone(char* str);

/* environment with outomatic cleanup !doesn't play nicely with break statements! */
#define with(var, init) for(var = init; var; var = realloc(var,0) )

/* make g-hash-table actually typable */
#define symt_new() g_hash_table_new(g_str_hash,g_str_equal)
#define symt_insert(table,name,sym) g_hash_table_insert(table,name,sym)
#define symt_foreach(table,func,datap) g_hash_table_foreach(table,func,datap)
#define symt_destroy(table) g_hash_table_destroy(table)
#define symt_get(table,name) g_hash_table_lookup(table,name)

