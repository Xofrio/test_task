#include "process.h"
#include "recommend.h"

struct object_data	g_data[amount_objects_maximum + 1];
char 				g_files[amount_files][size_file_name];
FILE * 				g_output;
pthread_cond_t 		g_condition; 
pthread_mutex_t 	g_mutex;
units 				g_time;
size_t 				g_amount_objects;
size_t              g_amount_observations;
bool 				g_write_happened;

int main
(
	int 	arguments_count,
	char * 	arguments[]
)
{
	pthread_t thread_process;
	pthread_t thread_recommend;

	g_amount_observations = g_write_happened = g_amount_objects = g_time = 0;

	for
	(
		size_t i = 0;
		i < amount_files;
		memset( g_files[i], 0, sizeof( g_files[i] )),
		strncpy
		(
			g_files[i],
			arguments_count > i + 1 ? arguments[i] : file_tag( i ),
			sizeof( g_files[i] ) - 1
		),
		++i
	);

	pthread_mutex_init( &g_mutex, NULL );
	pthread_cond_init( &g_condition, NULL );

	pthread_create( &thread_process, NULL, process, NULL ); 
	pthread_create( &thread_recommend, NULL, recommend, NULL );

	pthread_join( thread_process, NULL );
	pthread_join( thread_recommend, NULL );

	pthread_mutex_destroy( &g_mutex );
	pthread_cond_destroy( &g_condition );

	return EXIT_SUCCESS;
}
