#include "process.h"

void get_data
(
    FILE * const data,
    struct data_input input[][amount_objects_maximum + 1],
    size_t const o,
    size_t const i
)
{
    for
    (
        size_t j = 0;
        j < amount_input;
        fscanf( data, "%*s" ),
        fscanf
        (
            data,
            format( input[o][i].latitude ),
            !j          ? &input[o][i].latitude
            : j == 1    ? &input[o][i].longitude
            : j == 2    ? &input[o][i].altitude
                        : &input[o][i].time
        ),
        fscanf( data, "%*c" ),
        ++j
    );
}

void * process()
{
    struct data_input input[order][amount_objects_maximum + 1];
    FILE * data;
    size_t o = 0;
    size_t i;
    char has_other_objects;

    for (;;)
    {
        pthread_mutex_lock( &g_mutex );

        g_amount = has_other_objects = 0;

        data = fopen( g_file_input, "r" );
        
        fscanf( data, "%*c %*s %*c" );
        get_data( data, input, 0, g_amount ); // TODO: Change 0 to ORDER!

        fscanf( data, "%*c %*s %*c %c", &has_other_objects );
        for
        (
            ;
            has_other_objects == '{' && g_amount < amount_objects_maximum;
            get_data( data, input, 0, ++g_amount ),  // TODO: Change 0 to ORDER!
            fscanf( data, "%*s %c", &has_other_objects )
        );
        
        fclose( data );

        g_time = input[0][0].time;   // TODO: Change 0 to ORDER!
        
        // input[0][0 1 2 3] - me and my 3 doods measurements_0.
        //TEST (removing this)
        for
        (
            i = 0;
            i < g_amount;
            g_output[i].distance = input[0][0].latitude,
            g_output[i].approach_velocity = input[0][0].longitude,
            ++i
        );
        // TODO: Actual algorithm (:  

        o = (o + 1) % order;

        g_write_happened = true;
        pthread_cond_signal( &g_condition );
        pthread_mutex_unlock( &g_mutex );
    }
}
