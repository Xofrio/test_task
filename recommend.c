#include "recommend.h"

void * recommend()
{
    char const * recommendations[] =
    {
        "NO_THREAT",
        "WARNING",
        "CRITICAL"
    };
    size_t i;
    size_t j;

    // open Json here first to place a scratch
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
            i = j = 0;
            i < g_amount;
            j = g_output[i].distance < critical_distance( g_output[i].distance )
            ? 2
            : (g_output[i].distance < warning_distance_1( g_output[i].distance )
            && g_output[i].approach_velocity > warning_velocity( g_output[i].approach_velocity )
            || g_output[i].distance < warning_distance_2( g_output[i].distance )
            && g_output[i].approach_velocity < warning_velocity( g_output[i].approach_velocity )) && j < 2
            ? 1
            : j,
            ++i
        );

        // TODO: replace stdout with FILE handle (we need to g_output in json.)
        fprintf( stdout, "TIME:\t\t" );
        fprintf( stdout, format( g_time ), g_time );
        fprintf( stdout, "\tRECOMMENDATION:\t%s\n", recommendations[j] );

        g_write_happened = false;
        pthread_mutex_unlock( &g_mutex );

        sleep( amount_seconds_sleep ); // TODO: Change to usleep or keep it like this
    }
}
