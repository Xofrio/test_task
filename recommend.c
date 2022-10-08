#include "recommend.h"

/**
 * @brief   Signal interrupt handler that completes json
 *          and destroys mutex and conditional variable.
 */
void complete_json()
{
    // TODO:    might delete this if we need to always update output.json
    //          (1 recommendaion at a time, so we don't need to
    //          sort of "complete" the file by adding \n\t]\n}\n)
    g_output = fopen( g_files[file_index_output], "a+" );
    fprintf( g_output, "\n\t]\n}\n" );
    fclose( g_output );

    pthread_mutex_destroy( &g_mutex );
	pthread_cond_destroy( &g_condition );

    exit( EXIT_SUCCESS );
}

void * recommend()
{
    char const * recommendations[] =
    {
        "NO_THREAT",
        "WARNING",
        "CRITICAL"
    };
    struct sigaction    signal_interceptor;
    size_t              k = 0;
    size_t              i;
    size_t              j;

    memset( &signal_interceptor, 0, sizeof( signal_interceptor ) );
    signal_interceptor.sa_sigaction = complete_json;
    sigaction( SIGINT, &signal_interceptor, NULL );

    g_output = fopen( g_files[file_index_output], "w" );
    fprintf( g_output, "{\n\t\"recommendations\": [\n" );
    fclose( g_output );
    
    for (;;)
    {
        for
        (
            pthread_mutex_lock( &g_mutex ); 
            !g_write_happened;
            pthread_cond_wait( &g_condition, &g_mutex )
        );
        
        for
        (
            i = objects_start,
            j = 0;
            i < g_amount_objects + objects_start && j != 2;
            j =
            (
                g_data[i].distance < critical_distance

            ||  g_data[i].distance_guess < critical_distance
            )
            ? 2
            :
            (
                g_data[i].distance < warning_distance_1
            &&  g_data[i].approach_velocity > warning_velocity

            ||  g_data[i].distance < warning_distance_2
            &&  g_data[i].approach_velocity < warning_velocity

            ||  g_data[i].distance_guess < warning_distance_2
            &&  g_data[i].approach_velocity_guess > warning_velocity

            ||  g_data[i].distance_guess < warning_distance_2
            &&  g_data[i].approach_velocity_guess < warning_velocity
            ) && j < 2
            ? 1
            : j,
            ++i
        );

        g_output = fopen( g_files[file_index_output], "a+" );
        g_amount_observations++ > 0 && fprintf ( g_output, ",\n" );
        fprintf( g_output, "\t\t{\n\t\t\t\"time\": " );
        fprintf( g_output, format_output( g_time ), g_time );
        fprintf( g_output, ",\n\t\t\t\"recommendation\": \"%s\"\n\t\t}", recommendations[j] );
        fclose( g_output );

        if ( g_amount_observations == 10 ) raise(SIGINT);   // TODO: remove this.

        g_write_happened = false;
        pthread_mutex_unlock( &g_mutex );

        sleep( amount_seconds_sleep ); // TODO: Change to usleep or keep it like this
    }
}
