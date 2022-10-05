#include "process.h"

/**
 * @brief       Fills input array at specified order and specified object
 *              with data from specified json file.
 * @param input array to store data
 * @param data  json file containing data
 * @param index index of the current object
 */
void get_data
(
    struct object_data          input[],
    FILE *              const   data,
    size_t              const   index
)
{
    for
    (
        size_t j        = 0,
        current_order   = input[index].observations++ % order;
        j < amount_input;
        fscanf( data, "%*s" ),
        !j          ? fscanf_data( data, input, current_order, index, position.latitude )
        : j == 1    ? fscanf_data( data, input, current_order, index, position.longitude )
        : j == 2    ? fscanf_data( data, input, current_order, index, position.altitude )
                    : fscanf_data( data, input, current_order, index, time ),
        fscanf( data, "%*c" ),
        ++j
    );
}

/**
 * @brief           Computes distance between two objects.
 * @param first     first object
 * @param second    second object
 * @return          squared distance between two objects in 3d
 */
static units get_distance
(
    struct coordinates const * const first,
    struct coordinates const * const second
)
{
    units phi_1             = to_radians( first->longitude );
    units theta_1           = to_radians( first->latitude );
    units theta_2           = to_radians( second->latitude );
    units delta_altitude    = first->altitude - second->altitude;
    units distance_2D       =
    acos
    (
        sin( theta_1 ) * sin( theta_2 )
        + cos( theta_1 ) * cos( theta_2 ) * cos( phi_1 - to_radians( second->longitude ))
    ) * radius_earth( phi_1 );

    return distance_2D * distance_2D + delta_altitude * delta_altitude;
}

/**
 * @brief                   Computes approach velocity between two objects.
 * @param first_current     current observation of first object
 * @param first_previous    previous observation of first object
 * @param second_current    current observation of second object
 * @param second_previous   previous observation of second object
 * @return                  squared approach velocity between two objects in 3d
 */
static units get_approach_velocity
(
    struct data_input const * const first_current,
    struct data_input const * const first_previous,
    struct data_input const * const second_current,
    struct data_input const * const second_previous
)
{
    units delta_longitude_current       =
    first_current->position.longitude       - second_current->position.longitude;
    units delta_latitude_current        =
    first_current->position.latitude        - second_current->position.latitude;
    units delta_altitude_current        =
    first_current->position.altitude        - second_current->position.altitude;
    units radius_current                =
    radius_earth( delta_altitude_current )  + first_current->position.altitude;
    units sin_delta_latitude_current    =
    sin( to_radians( delta_latitude_current ));

    units delta_longitude_previous      =
    first_previous->position.longitude      - second_previous->position.longitude;
    units delta_latitude_previous       =
    first_previous->position.latitude       - second_previous->position.latitude;
    units delta_altitude_previous       =
    first_previous->position.altitude       - second_previous->position.altitude;
    units radius_previous               =
    radius_earth( delta_altitude_previous ) + first_previous->position.altitude;
    units sin_delta_latitude_previous   =
    sin( to_radians( delta_latitude_previous ));

    units delta_time                    =
    second_current->time                    - second_current->time;

    return
    (
        (
            delta_altitude_current * delta_altitude_current
        +   delta_longitude_current * delta_longitude_current * radius_current * radius_current
            * sin_delta_latitude_current * sin_delta_latitude_current
        +   delta_latitude_current * delta_latitude_current * radius_current * radius_current
        )
        - 
        (
            delta_altitude_previous * delta_altitude_previous
        +   delta_longitude_previous * delta_longitude_previous * radius_previous * radius_previous
            * sin_delta_latitude_previous * sin_delta_latitude_previous
        +   delta_latitude_previous * delta_latitude_previous * radius_previous * radius_previous
        )
    ) / ( delta_time * delta_time );
}

/**
 * @brief           Makes a guess coordinate based on given coordinates.
 * @param order_2   current observation
 * @param order_1   previous observation
 * @param order_0   previous to previous observation
 * @return          2nd order approximation - coordinate prediction
 */
static units make_guess
(
    units const order_2,
    units const order_1,
    units const order_0
)
{
    return
        two_point_five( order_2 ) * order_2
    -   ( order_1 + order_1 )
    +   point_five( order_0 ) * order_0;
}

/**
 * @brief           Compares id's of two objects of type object_data.
 * @param first     first object
 * @param second    second object
 * @return          integer < 0, if first id is lower,
 *                  integer = 0, if first id is equal to second,
 *                  integer > 0, if first id is higher
 */
static int object_id_ascending
(
    void const * const first,
    void const * const second
)
{
    return
        (( struct object_data * ) first )->id
    -   (( struct object_data * ) second )->id;
}

/**
 * @brief           Swaps two objects of type object_data.
 * @param first     first object
 * @param second    second object
 */
static void swap
(
    struct object_data * first,
    struct object_data * second
)
{
    struct object_data temp = *first;

    *first                  = *second;
    *second                 = temp;
}

/**
 * @brief           Searches for an object in specified range by specified id.
 * @param begin     begin of the range
 * @param end       end of the range (inclusive)
 * @param id        id with wich object should be found
 * @return          index of such an object or
 *                  index greater than maximum, if object does not exist
 */
static size_t find_by_id
(
    struct object_data * begin,
    struct object_data * end,
    size_t const id
)
{
    struct object_data * begin_initial = begin;
    for
    (
        struct object_data * middle;
        begin <= end;
        middle->id < id ? ( begin = middle + 1 ) : ( end = middle - 1 )
    )
    {
        middle = begin + ( end - begin ) / 2;

        if ( middle->id == id ) return middle - begin_initial + 1;
    }
    return amount_objects_maximum;
}

void * process()
{
    FILE *  data_input;
    size_t  current;
    size_t  id;
    size_t  i;
    size_t  amount_to_delete;
    size_t  self_order_current;
    size_t  self_order_previous;
    size_t  self_order_previous_to_previous;
    size_t  object_order_current;
    size_t  object_order_previous;
    size_t  object_order_previous_to_previous;
    char    has_other_objects;
    bool    got_new;

    for (;;)
    {
        pthread_mutex_lock( &g_mutex );

        amount_to_delete = has_other_objects = 0;

        data_input = fopen( g_files[file_index_input], "r" );
        fscanf( data_input, "%*c %*s %*c" );
        get_data( g_data, data_input, 0 );
        g_data[0].id = 0;   // TODO: might be unnecessary, so delete this
        fscanf( data_input, "%*c %*s %*c %c", &has_other_objects );
        got_new = false;
        for
        (
            ;
            has_other_objects == '{'
            && g_amount_objects < amount_objects_maximum;
            fscanf( data_input, "%*s %zu,", &id ),
            current =
            find_by_id
            (
                g_data + 1,
                g_data + g_amount_objects,
                id
            ),
            (
                current == amount_objects_maximum
                ?
                (
                    current                         = ++g_amount_objects,
                    g_data[current].id              = id,
                    g_data[current].observations    = 0,
                    got_new                         = true
                )
                :   0
            ),
            get_data( g_data, data_input, current ),
            got_new ? qsort( g_data, g_amount_objects + 1, sizeof( g_data[0] ), object_id_ascending ) : NULL,
            fscanf( data_input, "%*s %c", &has_other_objects ),
            got_new = false
        );
        fclose( data_input );

        // TODO: delete this
        // sleep( 3 );
    
        self_order_current              = ( g_data[0].observations + ( order - 1 )) % order;
        self_order_previous             = ( g_data[0].observations + ( order - 2 )) % order;
        self_order_previous_to_previous = g_data[0].observations % order;
        g_time                          = g_data[0].real[self_order_current].time;
        for
        (
            i = 1;
            i < g_amount_objects + 1;
            (
                g_data[i].real[( g_data[i].observations + ( order - 1 )) % order].time
            !=  g_time
            ?   ( ++amount_to_delete, g_data[i].id = __SIZE_MAX__ )
            :   0
            ),
            ++i
        );
        qsort( g_data, g_amount_objects + 1, sizeof( g_data[0] ), object_id_ascending );
        g_amount_objects -= amount_to_delete;

        if ( g_data[0].observations > 2 )
        {
            g_data[0].guess.position.latitude =
            make_guess
            (
                g_data[0].real[self_order_current].position.latitude,
                g_data[0].real[self_order_previous].position.latitude,
                g_data[0].real[self_order_previous_to_previous].position.latitude
            );
            g_data[0].guess.position.longitude =
            make_guess
            (
                g_data[0].real[self_order_current].position.longitude,
                g_data[0].real[self_order_previous].position.longitude,
                g_data[0].real[self_order_previous_to_previous].position.longitude
            );
            g_data[0].guess.position.altitude =
            make_guess
            (
                g_data[0].real[self_order_current].position.altitude,
                g_data[0].real[self_order_previous].position.altitude,
                g_data[0].real[self_order_previous_to_previous].position.altitude
            );
        }
        
        for
        (
            i = 1;
            i < g_amount_objects + 1;
            ++i
        )
        {
            object_order_current                = ( g_data[i].observations + ( order - 1 )) % order;
            object_order_previous               = ( g_data[i].observations + ( order - 2 )) % order;
            object_order_previous_to_previous   = g_data[i].observations % order;

            g_data[i].distance =
            get_distance
            (
                &g_data[0].real[self_order_current].position,
                &g_data[i].real[object_order_current].position
            );

            if ( g_data[i].observations > 1 )
            {
                g_data[i].approach_velocity =
                get_approach_velocity
                (
                    &g_data[0].real[self_order_current],
                    &g_data[0].real[self_order_previous],
                    &g_data[i].real[object_order_current],
                    &g_data[i].real[object_order_previous]
                );

                if ( g_data[i].observations > 2 )
                {
                    g_data[i].distance_guess =
                    get_distance
                    (
                        &g_data[0].guess.position,
                        &g_data[i].guess.position
                    );
                    
                    g_data[i].approach_velocity_guess =
                    get_approach_velocity
                    (
                        &g_data[0].guess,
                        &g_data[i].guess,
                        &g_data[0].real[self_order_previous],
                        &g_data[i].real[object_order_previous]
                    );
                    // TODO: add guess array
                    // Add +180 degrees EVERYWHERE for longitude
                }
            }
            else
            {
                g_data[i].approach_velocity = g_data[i].approach_velocity_guess = 0;
                g_data[i].distance_guess =
                warning_distance_1( g_time ) * warning_distance_1( g_time );
            }
        }

        g_write_happened = true;
        pthread_cond_signal( &g_condition );
        pthread_mutex_unlock( &g_mutex );
    }
}
