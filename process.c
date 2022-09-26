#include "process.h"

/**
 * @brief       Fills input array at specified order and specified object
 *              with data from specified json file
 * @param input array to store data
 * @param data  input json file
 * @param o     order
 * @param i     index of the current object
 */
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
        : j == 1    ? fscanf_data( input, o, i, longitude )
        : j == 2    ? fscanf_data( input, o, i, altitude )
                    : fscanf_data( input, o, i, time ),
        fscanf( data, "%*c" ),
        ++j
    );
}

/**
 * @brief           Fills input array at specified order and specified object
 *                  with data from specified json file
 * @param self      current object
 * @param object    other object
 * @return          squared distance between two objects in 3d
 */
static units get_distance
(
    struct data_input const * const self,
    struct data_input const * const object
)
{
    units phi_1 = to_radians( self->longitude );
    units phi_2 = to_radians( object->longitude );
    units theta_1 = to_radians( self->latitude );
    units theta_2 = to_radians( object->latitude );
    units delta_height = self->altitude - object->altitude;
    units distance_2D =
    acos_
    (
        sin_( theta_1 ) * sin_( theta_2 )
        + cos_( theta_1 ) * cos_( theta_2 ) * cos_( phi_1 - phi_2 )
    ) * radius_earth ( phi_1 );

    return distance_2D * distance_2D + delta_height * delta_height;
}

void * process()
{
    struct data_input   input[order][amount_objects_maximum + 1];
    FILE *              data_input;
    struct data_input   self;
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
            has_other_objects == '{'
            && g_amount_objects < amount_objects_maximum;
            get_data( input, data_input, 0, ++g_amount_objects ),  // TODO: Change 0 to ORDER!
            fscanf( data_input, "%*s %c", &has_other_objects )
        );
        
        fclose( data_input );

        g_time = input[0][0].time;  // TODO: Change first 0 to ORDER!
        self = input[0][0];         // TODO: Change first 0 to ORDER!
        
        // TODO: Actual algorithm (: [:(]
        for ( i = 0; i < g_amount_objects; ++i )
        {
            // if some condition - skip observation
            // ( I'm too stupid for extrapolation and splines :( )
            g_output[i].distance = get_distance( &self, &input[0][i + 1] );
            // What about velocity and it's equation?
        }
        
        o = (o + 1) % order;

        g_write_happened = true;
        pthread_cond_signal( &g_condition );
        pthread_mutex_unlock( &g_mutex );
    }
}
