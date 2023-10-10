Installing IOLITE
=================

This section guides you through IOLITE's simple installation process.

Installing portable builds
--------------------------

Installing IOLITE is trivial. Simply download a release from the `landing page <https://iolite-engine.com>`_ or, if you have subscribed to IOLITE PRO, from the `subscriber area <https://iolite-engine.com/subscribers>`_ and unzip it. Finally, launch the IOLITE executable. That's it. 🙂 

Installing Flatpak builds
-------------------------

In addition to the pre-packaged builds, IOLITE is available via our Flatpak repository. Flatpak builds provide automatic updates and also work on more uncommon Linux distributions. To add our repository as a remote, install IOLITE, and launch it, please perform the following steps:

.. code-block:: bash

  $ flatpak --user --no-gpg-verify remote-add md https://media.missing-deadlines.com/iolite/flatpak
  $ flatpak --user install md com.missing_deadlines.IOLITE
  $ flatpak run com.missing_deadlines.IOLITE
  $ # All data is stored in ~/.var/app/com.missing_deadlines.IOLITE/data/
  
Installing also adds a shortcut to IOLITE to your favorite launcher. After that, you can update IOLITE like any other Flatpak application:

.. code-block:: bash

  $ flatpak --user update com.missing_deadlines.IOLITE

In contrast to the portable builds, the Flatpak builds are also shipped with all the latest samples from the GitHub repository.

Beta releases (PRO only)
------------------------

Beta releases are released in between main releases for PRO subscribers. Please consider the following when working with a beta release of IOLITE:

.. important:: Beta releases are released more frequently and contain the latest features, but they can be a little less stable.

- Please switch to the ``develop`` branch in our public GitHub repository and search for a tag matching the version of IOLITE you are using
- Please ensure that you are using a matching version of the open source plugins. Due to potential API changes, there are separate plugin releases available for the beta versions of IOLITE