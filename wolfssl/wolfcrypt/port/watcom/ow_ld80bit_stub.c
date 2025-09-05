/* wolfssl/port/watcom/ow_ld80bit_stub.c
 *
 * Purpose: Provide the exact symbol `_SetLD80bit_` that Open Watcom's
 *          CRT (clbrdll) may reference via its `fltuse80` helper when
 *          linking DLLs. wolfSSL doesn't rely on FP precision, so a
 *          no-op is sufficient.
 *
 * Scope:   Compiled only under Open Watcom.
 */

#if defined(__WATCOMC__)

#ifdef __cplusplus
extern "C" {
#endif

/* Declare a C function named SetLD80bit... */
void SetLD80bit(void);

/* ...and force its external linkage name to be exactly `_SetLD80bit_`.
   This matches the symbol referenced by clbrdll.lib(fltuse80). */
#pragma aux SetLD80bit "_SetLD80bit_"

/* No-op definition. */
void SetLD80bit(void) { /* nothing */ }

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __WATCOMC__ */
