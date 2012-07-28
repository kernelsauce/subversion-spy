var Spy = {};

Spy.addListener = function(path){
    if (path && path.length > 0)
    {
        SpyWkBridge.addListenerPath(path);
    }
}

Spy.removeListener = function(path)
{
    if (path && path.length > 0)
    {
        SpyWkBridge.removeListenerPath(path);
    }
}

Spy.getAllNotifications = function()
{
    return SpyWkBridge.getAllNotifications();
}

Spy.getPollRate = function()
{
    return SpyWkBridge.getPollRate();
}

Spy.setPollRate = function(time)
{
    SpyWkBridge.setPollRate(time);
}

Spy.getListenerPaths = function()
{
    return SpyWkBridge.getListenerPaths();
}
