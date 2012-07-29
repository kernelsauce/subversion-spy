var Repository = {};

Repository.listenerTable = $("#listenerPathsBody");

Repository.updateListenerPaths = function()
{
    var currentPaths = Spy.getListenerPaths();
    Repository.listenerTable.empty();
    var listenerTableSize = currentPaths.length; // Repository.listenerTable.find(".icon-remove").parent().parent()
    if (listenerTableSize > 0)
    {
        var tableCell = document.createElement("td");
        var icon = document.createElement("i");
        for (var i = 0; i < listenerTableSize; i++)
        {
            var tableRow = document.createElement("tr");
            var pathName = tableCell.cloneNode();
            pathName.className = "listenerPath";
            pathName.innerHTML = currentPaths[i];
            var deleteButtonCell = tableCell.cloneNode();
            var deleteButton = icon.cloneNode();
            deleteButton.className = "icon-remove";
            deleteButtonCell.appendChild(deleteButton);
            tableRow.appendChild(pathName);
            tableRow.appendChild(deleteButtonCell);

            Repository.listenerTable.append(tableRow);
        }
    }
    else
    {
        Repository.listenerTable.html("<br /><strong>No Subversion Repositories added!</strong>");
    }
}

$("#addListenerAction").live("click", function(event){
    event.preventDefault();
    var pathToAdd = $("#listenerPath").val();
    Spy.addListener(pathToAdd);
    Repository.updateListenerPaths();
});

$("#browseForPath").live("click", function(event){
    event.preventDefault();
    Spy.addListener(Spy.openDirBrowser());
    Repository.updateListenerPaths();
});

Repository.listenerTable.find(".icon-remove").live("click", function(event){
    var removePath = $(this).parent().parent().find(".listenerPath").text();
    Spy.removeListener(removePath);
    Repository.updateListenerPaths();
});
