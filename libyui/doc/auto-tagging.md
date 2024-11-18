## Auto-tagging

Having git tags with version numbers helps external projects to work with our code. We have added Jenkins jobs to auto-tagging the most relevant *libyui* repositories:

* [libyui](https://ci.suse.de/job/libyui-libyui-tags-master/)
* [libyui-bindings](https://ci.suse.de/job/libyui-libyui-bindings-tags-master/)
* [libyui-ncurses](https://ci.suse.de/job/libyui-libyui-ncurses-tags-master/)
* [libyui-ncurses-pkg](https://ci.suse.de/job/libyui-libyui-ncurses-pkg-tags-master/)
* [libyui-qt](https://ci.suse.de/job/libyui-libyui-qt-tags-master/)
* [libyui-qt-pkg](https://ci.suse.de/job/libyui-libyui-qt-pkg-tags-master/)
* [libyui-qt-graph](https://ci.suse.de/job/libyui-libyui-qt-graph-tags-master/)

These Jenkins projects are automatically deployed by using [Jenkins Job Builder](https://docs.openstack.org/infra/jenkins-job-builder/). The projects are defined in the [tags-jobs.yaml](https://gitlab.suse.de/yast/infra/blob/master/jenkins/ci.suse.de/tags-jobs.yaml) file at our internal [infra repository](https://gitlab.suse.de/yast/infra).

Only version changes in *master* branches are automatically tagged. The tag needs to be generated manually when version is bumped for another branch:

```
rake version:tag
git push --tags
```

### Jenkins projects

Auto-tagging Jenkins projects are configured to add a new tag to the git repositories when a new version is indicated. The *rake* task `version:tag` is used to generate the new tag, see [tags.sh](https://gitlab.suse.de/yast/infra/blob/master/jenkins/ci.suse.de/tags.sh) script.

To deploy the auto-tagging Jenkins projects we have to run the following at the root of the [infra repository](https://gitlab.suse.de/yast/infra):

```
jenkins-jobs --conf jenkins/ci.suse.de.ini update jenkins/ci.suse.de/ '*-tags-*'
```

Note that write permissions are needed to push the tags to the GitHub repositories. But in this case, to configure special credentials is not needed to gain write permissions. The reason is that [ssh keys](https://gitlab.suse.de/yast/infra/tree/master/srv/salt/yast-jenkins/files) of [yast-i18n](https://github.com/orgs/libyui/people/yast-i18n) GitHub user are deployed to all Jenkins nodes. And that GitHub user belongs to the [libyui collaborators](https://github.com/orgs/libyui/people).
