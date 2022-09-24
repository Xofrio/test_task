#include "process.h"
#include "recommend.h"

struct data_output g_output[amount_objects_maximum];
char g_file_input[size_file_name];
char g_file_output[size_file_name];
pthread_mutex_t g_mutex;
pthread_cond_t g_condition;
FILE * g_data_output;
units g_time;
size_t g_amount_objects;
bool g_write_happened;

int main( int arguments_count, char * arguments[] )
{
	g_write_happened = g_amount_objects = g_time = 0;

	memset( g_file_input, 0, sizeof( g_file_input ) );
	memset( g_file_output, 0, sizeof( g_file_output ) );
	strncpy
	(
		g_file_input,
		arguments_count > 1 ? arguments[1] : "input.json",
		sizeof( g_file_input ) - 1
	);
	strncpy
	(
		g_file_output,
		arguments_count > 2 ? arguments[2] : "output.json",
		sizeof( g_file_output ) - 1
	);

	pthread_mutex_init( &g_mutex, NULL );
	pthread_cond_init( &g_condition, NULL );

	pthread_t thread_1;
	pthread_t thread_2;

	pthread_create( &thread_1, NULL, process, NULL ); 
	pthread_create( &thread_2, NULL, recommend, NULL );

	pthread_join( thread_1, NULL );
	pthread_join( thread_2, NULL );

	pthread_mutex_destroy( &g_mutex );
	pthread_cond_destroy( &g_condition );

	return 0;
}
