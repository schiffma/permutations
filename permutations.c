/*
** 2021-06-18   schiffma
**
** The author disclaims copyright to this source code.  In place of
** a legal notice, here is a blessing:
**
**    May you do good and not evil.
**    May you find forgiveness for yourself and forgive others.
**    May you share freely, never taking more than you give.
**
*************************************************************************
**
** This file implements a table-valued function:
**
**      select * from permutations('abcd');
**
** The function has a single (non-HIDDEN) column named permut that takes
** on all permutations of the string in its argument, including an empty string
** and the input string itself.  
*/
#if !defined(SQLITE_CORE) || !defined(SQLITE_OMIT_VIRTUALTABLE)
#if !defined(SQLITEINT_H)
#include "sqlite3ext.h"
#endif
SQLITE_EXTENSION_INIT1
#include <string.h>
#include <assert.h>




int factorial(int n)
{
  int f = 1;
  for (int c = 1; c <= n; c++)
    f = f * c;
  return f;
}


/* Modified permutation algorithm from 
https://github.com/Anjan50/https-www.hackerrank.com-challenges-permutations-of-strings-problem/blob/master/Permutations%20of%20Strings.c
*/


void swap2(char *s, int i, int j){
    char tmp = s[i];
    s[i] = s[j];
    s[j] = tmp;
}


void reverse2(char *s, int start, int end){
    while(start<end){
        swap2(s,start++,end--);
    }
}

int next_permutation2(int n, char *s)
{
    for(int i=n-2;i>-1;i--){
        if(s[i+1] > s[i]){
            /* get min max */
            for(int j=n-1;j>i;j--){
                if(s[j] > s[i]){
                    /* do swap */
                    swap2(s,i,j);
                    /* do reverse */
                    reverse2(s,i+1,n-1);
                    return 1;
                }
            }
        }
    }
    return 0;
}

/* permutations_vtab is a subclass of sqlite3_vtab which is
** underlying representation of the virtual table
*/
typedef struct permutations_vtab permutations_vtab;
struct permutations_vtab {
  sqlite3_vtab base;  /* Base class - must be first */
  /* No additional fields are necessary */
};

/* permutations_cursor is a subclass of sqlite3_vtab_cursor which will
** serve as the underlying representation of a cursor that scans
** over rows of the result
*/
typedef struct permutations_cursor permutations_cursor;
struct permutations_cursor {
  sqlite3_vtab_cursor base;  /* Base class - must be first */
  sqlite3_int64 iRowid;      /* The rowid */
  char *zStr;                /* Original string to be permutated */
  int nStr;                  /* Length of the string in bytes */
  int nRows;				 /* Number of rows, calculated based on input length */
};

/*
** The permutationsConnect() method is invoked to create a new
** template virtual table.
**
** Think of this routine as the constructor for permutations_vtab objects.
**
** All this routine needs to do is:
**
**    (1) Allocate the permutations_vtab object and initialize all fields.
**
**    (2) Tell SQLite (via the sqlite3_declare_vtab() interface) what the
**        result set of queries against the virtual table will look like.
*/
static int permutationsConnect(
  sqlite3 *db,
  void *pAux,
  int argc, const char *const*argv,
  sqlite3_vtab **ppVtab,
  char **pzErr
){
  permutations_vtab *pNew;
  int rc;

  rc = sqlite3_declare_vtab(db,
           "CREATE TABLE permutations(permut TEXT, original_string TEXT HIDDEN)"
       );
  if( rc==SQLITE_OK ){
    pNew = sqlite3_malloc( sizeof(*pNew) );
    *ppVtab = (sqlite3_vtab*)pNew;
    if( pNew==0 ) return SQLITE_NOMEM;
    memset(pNew, 0, sizeof(*pNew));
    sqlite3_vtab_config(db, SQLITE_VTAB_INNOCUOUS);
  }
  return rc;
}

/*
** This method is the destructor for permutations_vtab objects.
*/
static int permutationsDisconnect(sqlite3_vtab *pVtab){
  permutations_vtab *p = (permutations_vtab*)pVtab;
  sqlite3_free(p);
  return SQLITE_OK;
}

/*
** Constructor for a new permutations_cursor object.
*/
static int permutationsOpen(sqlite3_vtab *p, sqlite3_vtab_cursor **ppCursor){
  permutations_cursor *pCur;
  pCur = sqlite3_malloc( sizeof(*pCur) );
  if( pCur==0 ) return SQLITE_NOMEM;
  memset(pCur, 0, sizeof(*pCur));
  *ppCursor = &pCur->base;
  return SQLITE_OK;
}

/*
** Destructor for a permutations_cursor.
*/
static int permutationsClose(sqlite3_vtab_cursor *cur){
  permutations_cursor *pCur = (permutations_cursor*)cur;
  sqlite3_free(pCur->zStr);
  sqlite3_free(pCur);
  return SQLITE_OK;
}


/*
** Advance a permutations_cursor to its next row of output.
*/
static int permutationsNext(sqlite3_vtab_cursor *cur){
  permutations_cursor *pCur = (permutations_cursor*)cur;
  pCur->iRowid++;
  return SQLITE_OK;
}

/*
** Return values of columns for the row at which the permutations_cursor
** is currently pointing.
*/
static int permutationsColumn(
  sqlite3_vtab_cursor *cur,   /* The cursor */
  sqlite3_context *ctx,       /* First argument to sqlite3_result_...() */
  int i                       /* Which column to return */
){
  permutations_cursor *pCur = (permutations_cursor*)cur;
  if (pCur->iRowid > 0 ){
	 int r  = next_permutation2((int)pCur->nStr, pCur->zStr); 

  } 
  sqlite3_result_text(ctx, pCur->zStr, pCur->nStr, 0);                      
  return SQLITE_OK;
}

/*
** Return the rowid for the current row.  In this implementation, the
** rowid is the same as the output value.
*/
static int permutationsRowid(sqlite3_vtab_cursor *cur, sqlite_int64 *pRowid){
  permutations_cursor *pCur = (permutations_cursor*)cur;
  *pRowid = pCur->iRowid;
  return SQLITE_OK;
}

/*
** Return TRUE if the cursor has been moved off of the last
** row of output.
*/
static int permutationsEof(sqlite3_vtab_cursor *cur){
  permutations_cursor *pCur = (permutations_cursor*)cur;
  return pCur->iRowid >= pCur->nRows;
}

/*
** This method is called to "rewind" the permutations_cursor object back
** to the first row of output.  This method is always called at least
** once prior to any call to permutationsColumn() or permutationsRowid() or 
** permutationsEof().
*/
static int permutationsFilter(
  sqlite3_vtab_cursor *pVtabCursor, 
  int idxNum, const char *idxStr,
  int argc, sqlite3_value **argv
){
  permutations_cursor *pCur = (permutations_cursor *)pVtabCursor;
  sqlite3_free(pCur->zStr);
  if (argc>0 && strlen(sqlite3_value_text(argv[0])) <= 10) {
    pCur->zStr = sqlite3_mprintf("%s", sqlite3_value_text(argv[0]));    
	pCur->nStr = pCur->zStr ? (int)(strlen(pCur->zStr)) : 0;
	pCur->nRows = pCur->zStr ? factorial((int)strlen(pCur->zStr)) : 0;
  } else {
    pCur->zStr = 0;
    pCur->nStr = 0;
  }
  pCur->iRowid = 0;
  return SQLITE_OK;
}

/*
** SQLite will invoke this method one or more times while planning a query
** that uses the virtual table.  This routine needs to create
** a query plan for each invocation and compute an estimated cost for that
** plan.
*/
static int permutationsBestIndex(
  sqlite3_vtab *tab,
  sqlite3_index_info *pIdxInfo
){
  /* Search for a usable equality constraint against column 1 
  ** (original_string) and use it if at all possible */
  int i;
  const struct sqlite3_index_constraint *p;

  for(i=0, p=pIdxInfo->aConstraint; i<pIdxInfo->nConstraint; i++, p++){
    if( p->iColumn!=1 ) continue;
    if( p->op!=SQLITE_INDEX_CONSTRAINT_EQ ) continue;
    if( !p->usable ) continue;
    pIdxInfo->aConstraintUsage[i].argvIndex = 1;
    pIdxInfo->aConstraintUsage[i].omit = 1;
    pIdxInfo->estimatedCost = (double)10;
    pIdxInfo->estimatedRows = 10;
    return SQLITE_OK;
  }
  pIdxInfo->estimatedCost = (double)1000000000;
  pIdxInfo->estimatedRows = 1000000000;
  return SQLITE_OK;
}

/*
** This following structure defines all the methods for the 
** virtual table.
*/
static sqlite3_module permutationsModule = {
  /* iVersion    */ 0,
  /* xCreate     */ 0,
  /* xConnect    */ permutationsConnect,
  /* xBestIndex  */ permutationsBestIndex,
  /* xDisconnect */ permutationsDisconnect,
  /* xDestroy    */ 0,
  /* xOpen       */ permutationsOpen,
  /* xClose      */ permutationsClose,
  /* xFilter     */ permutationsFilter,
  /* xNext       */ permutationsNext,
  /* xEof        */ permutationsEof,
  /* xColumn     */ permutationsColumn,
  /* xRowid      */ permutationsRowid,
  /* xUpdate     */ 0,
  /* xBegin      */ 0,
  /* xSync       */ 0,
  /* xCommit     */ 0,
  /* xRollback   */ 0,
  /* xFindMethod */ 0,
  /* xRename     */ 0,
  /* xSavepoint  */ 0,
  /* xRelease    */ 0,
  /* xRollbackTo */ 0,
  /* xShadowName */ 0
};


#ifdef _WIN32
__declspec(dllexport)
#endif
int sqlite3_permutations_init(
  sqlite3 *db, 
  char **pzErrMsg, 
  const sqlite3_api_routines *pApi
){
  int rc = SQLITE_OK;
  SQLITE_EXTENSION_INIT2(pApi);
  rc = sqlite3_create_module(db, "permutations", &permutationsModule, 0);
  return rc;
}
#endif /* !defined(SQLITE_CORE) || !defined(SQLITE_OMIT_VIRTUALTABLE) */
