/*
 * code_standard.c
 * 
 * Follow the following practice in writing GNU source code
 * 
 * Reference: GNU Coding Standard, http://www.gnu.org/prep/standards
 * 
 */

#if 0
/*
 * ==START OF FILE==
 * *File name
 * *A line or two about the overall purpose of the file
 */
/*
 * foo.c
 * driver for ethernet chips (DM9000A)
 */


/*
 * ==FUNCTION HEADER==
 */
/**
 * \brief swap two integer variables
 * \param a pointer to first variable
 * \param b pointer to second variable
 * \retval 0 on success
 * \retval -1 on failure
 */


/*
 * ==DECLARING Global or static VARAIBLE==
 * *There should be a comment on each static and global variable
 * *Avoid upper case letters, use lower case letters and underscore
 */
/* Nonzero means truncate lines in the display;
   zero means continue them.  */
static int truncate_lines;


/*
 * ==FUNCTION==
 * *Return type on new line
 * *Function name on new line
 * *Open-brace on new line
 */
static char *
foo (char *s1, char *s2)
{
  //...
}


/*
 * ==COMMENTS WITHIN FUNCTIONS==
 * *use capitial letters to indicate the value in a variable
 */
  int node;

  //Search for all NODE
  for(node=0; node<MAX; node++){
    //...
  } 


/*
 * ==CODE==
 * *Each level of indentation consists of 2 spaces '{'
 * *Add space before open-parentheses '('
 * *Add space after comma ','
 */
  if (x < foo (y, z))
    haha = bar[4] + 5;
  else
  {
    while (z)
    {
      haha += foo (z, z);
      z--;
    }
    return ++x + bar ();
  }


/*
 * ==EXPRESSION ON MULTIPLE LINES==
 * *Split it before an operator, not after one
 */
  if (foo_this_is_long && bar > win (x, y, z)
      && remaining_condition)


/*
 * ==DO-WHILE LOOP==
 * *Use the following format
 */
  do
    {
      a = foo (a);
    }
  while (a > 0);


/*
 * ==#else-#endif==
 * *Every ．#endif・ should have a comment
 * *The comment should state the condition of the conditional that is ending, including its sense.
 * *．#else・ should have a comment describing the condition and sense of the code that follows. 
 */
#ifdef foo
  //...
#else /* not foo */
  //...
#endif /* not foo */

#ifdef foo
  //...
#endif /* foo */

#ifndef foo
  //...
#else /* foo */
  //...
#endif /* foo */

#ifndef foo
  //...
#endif /* not foo */


/*
 * ==#define-enum==
 * *When you want to define names with constant integer values, use enum rather than ．#define・
 */
/**
 * \enum SMTP_ERR
 * SMTP ERROR code
 */
enum SMTP_ERR 
{ 
  /** No error */
  SMTP_ERR_OK, 
  /** Timeout */
  SMTP_ERR_TIMEOUT, 
  /** Mail server not ready */
  SMTP_ERR_NOT_READY,
  /** Error in HELO reply */
  SMTP_ERR_HELO,
  /** Sender address unavailable */
  SMTP_ERR_MAIL_FROM,
  /** Recepient address unavailable */
  SMTP_ERR_RCPT_TO,
  /** Cannot start sending data */
  SMTP_ERR_DATA_START,
  /** Cannot stop sending data */
  SMTP_ERR_DATA_END,
  /** AUTH LOGIN not supported */
  SMTP_ERR_AUTH_LOGIN,
  /** Invalid login name */
  SMTP_ERR_USERNAME,
  /** Invalid password */
  SMTP_ERR_PASSWORD
};

#endif /* do not compile */

