#include "recommend.h"

void complete_json()
{
    g_data_output = fopen( g_file_output, "a+" );
    fprintf( g_data_output, "\n\t]\n}\n" );
    fclose( g_data_output );

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
    size_t k = 0;
    size_t i;
    size_t j;

    struct sigaction signal_interceptor;
    memset( &signal_interceptor, 0, sizeof( signal_interceptor ) );
    signal_interceptor.sa_sigaction = complete_json;
    sigaction( SIGINT, &signal_interceptor, NULL );

    g_data_output = fopen( g_file_output, "w" );
    fprintf( g_data_output, "{\n\t\"recommendations\": [\n" );
    fclose( g_data_output );
    
    for (;;)
    {
        for
        (
            pthread_mutex_lock( &g_mutex ); 
            !g_write_happened;
            pthread_cond_wait( &g_condition, &g_mutex )
        );
        // TODO: Critical means we need to break from for, right?
        for
        (
            i = j = 0;
            i < g_amount_objects;
            j = g_output[i].distance < critical_distance( g_output[i].distance )
            ? 2
            : (g_output[i].distance < warning_distance_1( g_output[i].distance )
            && g_output[i].approach_velocity > warning_velocity( g_output[i].approach_velocity )
            || g_output[i].distance < warning_distance_2( g_output[i].distance )
            && g_output[i].approach_velocity < warning_velocity( g_output[i].approach_velocity )) 
            && j < 2
            ? 1
            : j,
            ++i
        );

        g_data_output = fopen( g_file_output, "a+" );
        k++ > 0 && fprintf ( g_data_output, ",\n" );
        fprintf( g_data_output, "\t\t{\n\t\t\t\"time\": " );
        fprintf( g_data_output, format( g_time ), g_time );
        fprintf( g_data_output, ",\n\t\t\t\"recommendation\": \"%s\"\n\t\t}", recommendations[j] );
        fclose( g_data_output );

        g_write_happened = false;
        pthread_mutex_unlock( &g_mutex );

        sleep( amount_seconds_sleep ); // TODO: Change to usleep or keep it like this
    }
}
