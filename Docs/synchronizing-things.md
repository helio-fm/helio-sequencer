# Synchronizing things across devices

## Version control

The concept of [version control](https://en.wikipedia.org/wiki/Version_control) comes from the software development world. If you're not familiar with it, you can think of it as of creating "savepoints" in your project, and tracking changes made since the last time you saved.

The point of having a version control is lowering the friction of committing changes: any changeset can be reset if you don't like it, any saved revision can be restored later. Helio was started as a prototyping tool, a playground for ideas, where you'd want to have several variations of your sketch — hence the idea of having a built-in version control.

Notable use cases are:

 * saving a project state at some point, and resetting to any earlier saved state,
 * resetting some of the recent changes (think of it as of another mechanism of undoing things),
 * hiding/unhiding recent changes to get the idea of what have been done since the last commit,
 * synchronizing projects across devices.

The UI of the versions page is split in two parts:

![version-control]

The left side lists all changes in the project, compared to the current revision. Items in the list can be checked and unchecked: both committing and resetting changes are applied selectively.

The right side shows the tree of all revisions that you have saved. Note a couple of icons under each revision: they indicate whether the revision is available locally, or remotely, or both.

### Synchronizing projects

// TODO

### Synchronizing custom configs

// TODO

[version-control]: images/version-control.png "The version control page"
