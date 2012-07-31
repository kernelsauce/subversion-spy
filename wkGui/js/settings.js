var Settings = {};

// Defines
Settings.pollRate = $("#pollRate");
Settings.saveAction = $("#setSettings");
// end of defines.

/** On load **/
Settings.pollRate.val(Spy.getPollRate());

Settings.saveAction.live("click", function(event){
    event.preventDefault();
    var pollRateNumber = parseInt(Settings.pollRate.val());
    Spy.setPollRate(pollRateNumber);
})

