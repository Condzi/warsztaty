
/*
 * Writes error message to stderr. 
*/
#ifndef REPORT_ERROR
	#define REPORT_ERROR(message) fprintf(stderr, "!!! " message "\n")
#endif