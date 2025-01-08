/**
 * Create a table for ev.Periodic that gives access to the constructor for
 * periodic objects.
 *
 * [-0, +1, ?]
 */
static int luaopen_ev_periodic(lua_State *L) {
    lua_pop(L, create_periodic_mt(L));

    lua_createtable(L, 0, 1);

    lua_pushcfunction(L, periodic_new);
    lua_setfield(L, -2, "new");

    return 1;
}

/**
 * Create the periodic metatable in the registry.
 *
 * [-0, +1, ?]
 */
static int create_periodic_mt(lua_State *L) {

    static luaL_Reg fns[] = {
        { "again",         periodic_again },
        { "at",            periodic_at },
        { "stop",          periodic_stop },
        { "start",         periodic_start },
        { NULL, NULL }
    };
    luaL_newmetatable(L, PERIODIC_MT);
    add_watcher_mt(L);
    luaL_setfuncs(L, fns, 0);

    return 1;
}

/**
 * Create a new periodic object.  Arguments:
 *   1 - callback function.
 *
 * @see watcher_new()
 *
 * [+1, -0, ?]
 */
static int periodic_new(lua_State* L) {
    ev_tstamp offset  = luaL_checknumber(L, 2);
    ev_tstamp interval = luaL_optnumber(L, 3, 0);
    //const char *reschedule_cb = luaL_checkstring(L, 4); //PP manual reschedule mode currently not supported
    ev_periodic*  periodic;

    periodic = watcher_new(L, sizeof(ev_periodic), PERIODIC_MT);
    //ev_periodic_init(periodic, &periodic_cb, offset, interval, reschedule_cb );
    ev_periodic_init(periodic, &periodic_cb, offset, interval, 0 );
    return 1;
}

/**
 * @see watcher_cb()
 *
 * [+0, -0, m]
 */
static void periodic_cb(struct ev_loop* loop, ev_periodic* periodic, int revents) {
    watcher_cb(loop, periodic, revents);
}

/**
 * Simply stops and restarts the periodic watcher again.
 *
 *   1 - timer object.
 *   2 - loop object.
 *
 * Usage:
 *    periodic:again(loop)
 *
 * [+0, -0, e]
 */
static int periodic_again(lua_State *L) {
    ev_periodic*       periodic  = check_periodic(L, 1);
    struct ev_loop* loop      = *check_loop_and_init(L, 2);

	ev_periodic_again(loop, periodic);
	loop_start_watcher(L, 2, 1, -1);

    return 0;
}

/**
 * When active, returns the absolute time that the watcher is supposed to
 * trigger next.
 *
 * Usage:
 *    periodic:at()
 *
 * [+0, -0, e]
 */
static int periodic_at(lua_State *L) {
    ev_periodic*       periodic  = check_periodic(L, 1);

    return 0;
}

/**
 * Stops the periodic so it won't be called by the specified event loop.
 *
 * Usage:
 *     periodic:stop(loop)
 *
 * [+0, -0, e]
 */
static int periodic_stop(lua_State *L) {
    ev_periodic*       periodic  = check_periodic(L, 1);
    struct ev_loop* loop   = *check_loop_and_init(L, 2);

    loop_stop_watcher(L, 2, 1);
    ev_periodic_stop(loop, periodic);

    return 0;
}

/**
 * Starts the periodic so it won't be called by the specified event loop.
 *
 * Usage:
 *     periodic:start(loop [, is_daemon])
 *
 * [+0, -0, e]
 */
static int periodic_start(lua_State *L) {
    ev_periodic*       periodic  = check_periodic(L, 1);
    struct ev_loop* loop   = *check_loop_and_init(L, 2);
    int is_daemon          = lua_toboolean(L, 3);

    ev_periodic_start(loop, periodic);
    loop_start_watcher(L, 2, 1, is_daemon);

    return 0;
}
