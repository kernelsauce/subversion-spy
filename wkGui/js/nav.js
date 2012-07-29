var nav = {};
nav.statusPage = $("#status");
nav.listenersPage = $("#listeners");
nav.logsPage = $("#logs");
nav.settingsPage = $("#settings");
nav.helpPage = $("#help");

nav.goto = function(page)
{
    $(".maincontent").hide();
    page.show();
}

nav.goto(nav.listenersPage);
