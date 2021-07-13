// The verbose/output messages are formatted as:
// <operation> | <key1>: <value1> - <key2>: <value2> - ... - <keyN>: <valueN>;
// Example:
// compute	| buffer occupation: 10976/400000 - in/out: 10976/0 - last/state: init/init;
#ifndef VERSION
#define VERSION "0.4.1"
#endif

#ifndef STDERR_ERROR
#define STDERR_ERROR "\033[31mError!\033[0m "
#endif

#ifndef FILE_NOT_FOUND_EXCEPTION
#define FILE_NOT_FOUND_EXCEPTION 1010
#endif

#ifndef FILE_PARSE_ERROR
#define FILE_PARSE_ERROR 1011
#endif

#ifndef FILE_UNABLE_TO_WRITE
#define FILE_UNABLE_TO_WRITE 1012
#endif

#ifndef VALUE_ERROR_EXCEPTION
#define VALUE_ERROR_EXCEPTION 1020
#endif

#ifndef VERBOSE_NONE
#define VERBOSE_NONE 0      // Didn't print and returns the buffer status integer
#endif

#ifndef VERBOSE_MINIMAL
#define VERBOSE_MINIMAL 1   // Only prints the buffer final status (overflow, underflow or done)
#endif

#ifndef VERBOSE_STANDARD
#define VERBOSE_STANDARD 2
#endif


#ifndef VERBOSE_EXTRA
#define VERBOSE_EXTRA 3
#endif

#ifndef VERBOSE_ALL
#define VERBOSE_ALL 4
#endif

#ifndef SEPARATOR_41
#define SEPARATOR_41 "*****************************************"
#endif

#ifndef STDOUT_PARSE
#define STDOUT_PARSE "parse\t| "
#endif

#ifndef STDOUT_COMPUTE
#define STDOUT_COMPUTE "compute\t| "
#endif

#ifndef STDOUT_OUTPUT
#define STDOUT_OUTPUT "output\t| "
#endif

#ifndef STDOUT_WARNING
#define STDOUT_WARNING "warning\t| "
#endif

#ifndef CSV_FILE_SEPARATOR
#define CSV_FILE_SEPARATOR ", "
#endif

#ifndef PYTHON_PLOT_SCRIPT
#define PYTHON_PLOT_SCRIPT

#include <string>
std::string PYTHON_COMMAND(std::string input_file, std::string output_file, std::string python_path = "python3");
#endif

#ifndef BITS_MAX
#define BITS_MAX 1000001
#endif
