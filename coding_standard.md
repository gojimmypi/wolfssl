checklist:

- [] Emphasis on readability and understandability.
- [] Limit lines to 80 characters long.
- [] Proper indenting. Exactly 4 spaces, no tabs.
- [] No double-slash comments. /* only */
- [] Zero special characters, no unicode. Beware of PDF copy/paste.
- [] See template for example CoolThing() function.  
- [] All functions should return a value.  No void function returns.
- [] Functions with no arguments must have (void) in arg list.
- [] No void returns.
- [] No types in names.
- [] No recursion.
- [] No C standard library assumption.
- [] No build warnings. (Jenkins treats warnings as errors)
- [] No magic numbers. If a numeric value is hard-coded is should be clearly documented.
- [] All conditional / loops use {brackets}, even for just one line.
- [] Try to limit functions to 100 lines.
- [] Check for TODO items that need to be addressed.
- [] Emphasis on readability and understandability.

Guidlines:

Generous use of brackets and parenthesis: emphasize readability over cleverness.
Follow the patterns in existence. 

```
/*
** CoolThing - CamelCase naming. 
**
** Every function should have a description of what it does.
** 
** Note bracket {} positions in sample, below: 
*/
int CoolThing(int* param) 
{
    int rc; /* return code */

    /* all valiables declared at top */
    int idx; 
    int ctx = 0; /* don't forget init */

    if (param == NULL) {
        return BAD_FUNC_ARG;
    }    
    
    rc = do_func(ctx)

    if (rc == 0) {
        rc = do_func2()
    }

    if (rc == 0) {
        rc = do_func3()
    }

    return rc;
}
```
