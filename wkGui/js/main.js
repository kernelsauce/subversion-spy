function main() {
    // Go to listeners page, and update listeners.
    nav.goto(nav.listenersPage, $("#svnrepos_link"));
    Repository.updateListenerPaths();
}
