var Logs = {};
Logs.repositoryLogsList = $("#repository_logs_list");


Logs.listRepositories = function(){
            Logs.repositoryLogsList.empty();
            var paths = Spy.getListenerPaths() || [];
            for (var i = 0; paths.length > i; i++){
                Logs.repositoryLogsList.append('<li><button class="btn btn-mini btn-info">' + paths[i] + '</button></li>');
            }
        }

Logs.listRepositories();
