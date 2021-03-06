# Build Switch Images - buildimage

# Description
Build an [Open Network Install Environment (ONIE)](https://github.com/opencomputeproject/onie) compatiable network operating system (NOS) installer image for network switches, and also build docker images running inside the NOS.

# Usage
## Build NOS installer image

    ./build_debian USERNAME PASSWORD_ENCRYPTED && ./build_image.sh
    
For example, the user name is 'acsadmin' and the password is 'YourPaSsWoRd'.

    ./build_debian.sh "acsadmin" "$(perl -e 'print crypt("YourPaSsWoRd", "salt"),"\n"')" && ./build_image.sh

The root is disabled, but the created user could sudo.


## Build docker images

    ./build_docker.sh docker-sswsyncd
    ./build_docker.sh docker-database
    ./build_docker.sh docker-bgp
    ./build_docker.sh docker-snmp
    ./build_docker.sh docker-lldp

# Contribution guide

All contributors must sign a contribution license agreement before contributions can be accepted.  Contact kasubra@microsoft.com or daloher@microsoft.com.  Later this will be automated.

### GitHub Workflow

We're following basic GitHub Flow. If you have no idea what we're talking about, check out [GitHub's official guide](https://guides.github.com/introduction/flow/). Note that merge is only performed by the repository maintainer.

Guide for performing commits:

* Isolate each commit to one component/bugfix/issue/feature
* Use a standard commit message format:

>     [component/folder touched]: Description intent of your changes
>
>     [List of changes]
>
> 	  Signed-off-by: Your Name your@email.com

For example:

>     swss-common: Stabilize the ConsumerTable
>
>     * Fixing autoreconf
>     * Fixing unit-tests by adding checkers and initialize the DB before start
>     * Adding the ability to select from multiple channels
>     * Health-Monitor - The idea of the patch is that if something went wrong with the notification channel,
>       we will have the option to know about it (Query the LLEN table length).
>
>       Signed-off-by: user@dev.null


* Each developer should fork this repository and [add the team as a Contributor](https://help.github.com/articles/adding-collaborators-to-a-personal-repository)
* Push your changes to your private fork and do "pull-request" to this repository
* Use a pull request to do code review
* Use issues to keep track of what is going on
