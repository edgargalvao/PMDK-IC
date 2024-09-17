#ifdef __cplusplus
extern "C" {
#endif

void assoc_init(void);
item *assoc_find(const char *key, const size_t nkey);
int assoc_insert(item *item);
void assoc_delete(const char *key, const size_t nkey);

#ifdef __cplusplus
}
#endif
