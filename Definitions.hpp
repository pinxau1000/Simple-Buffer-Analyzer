// The verbose/output messages are formatted as:
// <operation> | <key1>: <value1> - <key2>: <value2> - ... - <keyN>: <valueN>;
// Example:
// compute	| buffer occupation: 10976/400000 - in/out: 10976/0 - last/state: init/init;
#define VERSION "0.2.2"

#define ERROR "\033[31mError!\033[0m "
#define FILE_NOT_FOUND_EXCEPTION 1010
#define FILE_PARSE_ERROR 1011
#define FILE_UNABLE_TO_WRITE 1012
#define VALUE_ERROR_EXCEPTION 1020

#define VERBOSE_STANDARD 0
#define VERBOSE_EXTRA 1
#define VERBOSE_ALL 2


#define SEPARATOR_9 "*********"
#define SEPARATOR_41 "*****************************************"
#define INFO "info\t| "
#define PARSE "parse\t| "
#define COMPUTE "compute\t| "
#define OUTPUT "output\t| "

#define CSV_FILE_SEPARATOR ", "
