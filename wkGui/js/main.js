function main() {
    // Go to listeners page, and update listeners.
    nav.goto(nav.listenersPage);
    Repository.updateListenerPaths();
}
