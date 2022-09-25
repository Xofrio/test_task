#include "process.h"

void get_data
(
    struct data_input           input[][amount_objects_maximum + 1],
    FILE *              const   data,
    size_t              const   o,
    size_t              const   i
)
{
    for
    (
        size_t j = 0;
        j < amount_input;
        fscanf( data, "%*s" ),
        !j          ? fscanf_data( input, o, i, latitude )
        : j == 1    ? fscanf_data( input, o, i, longtitude )
        : j == 2    ? fscanf_data( input, o, i, altitude )
                    : fscanf_data( input, o, i, time ),
        fscanf( data, "%*c" ),
        ++j
    );
}

void * process()
{
    struct data_input   input[order][amount_objects_maximum + 1];
    FILE *              data_input;
    size_t              o = 0;
    size_t              i;
    char                has_other_objects;

    for (;;)
    {
        pthread_mutex_lock( &g_mutex );

        g_amount_objects = has_other_objects = 0;

        data_input = fopen( g_files[file_index_input], "r" );
        
        fscanf( data_input, "%*c %*s %*c" );
        get_data( input, data_input, 0, g_amount_objects ); // TODO: Change 0 to ORDER!

        fscanf( data_input, "%*c %*s %*c %c", &has_other_objects );
        for
        (
            ;
            has_other_objects == '{' && g_amount_objects < amount_objects_maximum;
            get_data( input, data_input, 0, ++g_amount_objects ),  // TODO: Change 0 to ORDER!
            fscanf( data_input, "%*s %c", &has_other_objects )
        );
        
        fclose( data_input );

        g_time = input[0][0].time;   // TODO: Change 0 to ORDER!
        
        // TODO: Actual algorithm (:  

        o = (o + 1) % order;

        g_write_happened = true;
        pthread_cond_signal( &g_condition );
        pthread_mutex_unlock( &g_mutex );
    }
}
