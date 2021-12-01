# Contributing to the User Guide

Our User Guide is written in markdown and converted to HTML using [MkDocs](https://www.mkdocs.org/).

### Directory Info

```
📂 app-scripts/   ~ Scripts used for CI/CD deployment phase
📂 docs-source/   ~ Markdown files and web-ready images for User Guide
📂 image-source/  ~ Editable source files for images for User Guide
📄 appspec.yml    ~ App deployment config used for CI/CD deployment phase
📄 mkdocs.yml     ~ MkDocs config file used when generating HTML docs
```



## Getting Started

In order to view the HTML version of the docs locally or to contribute new documentation, you'll need to install the MkDocs tool...

```
$ pip install mkdocs
```



To begin viewing or editing the docs...

```
$ cd {repo}/docs/user-guide
$ mkdocs serve
```
If the above command does not work, try:
```
$ python -m mkdocs serve
````

Then, open http://127.0.0.1:8000 in your browser to view the generated HTML docs. As you edit the markdown source files your edits will automatically be reflected in the browser.

To add a new page to the docs, simply create a new markdown file under *{repo}***/docs/user-guide/docs/** and update the *{repo}***/docs/user-guide/mkdocs.yml** file to reference it.



## Special Features

We've customize our MkDocs theme to enable a few enhanced formatting features:

### **Number Badges**

You can format a number so that it appears as a number badge (a number in circle) by using the following markup in your markdown:

```
<number-badge>1</number-badge>
```

Example:

```
This text includes <number-badge>1</number-badge> example of a number badge!
```

## Publishing Changes

Once documentation changes have been approved, you can utilize the command to publish them to our GitHub Page. 

```
$ python -m mkdocs gh-deploy
```

This command commits and pushes the changes directly to the `gh-pages` repo, which is used to host the GitHub Page. So this should only be used once everything has been fully verified.