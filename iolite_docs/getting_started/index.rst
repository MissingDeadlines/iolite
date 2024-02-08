👀 Getting started
==================

This section contains installation instructions and guides to get you started.

Installing IOLITE
-----------------

.. note:: IOLITE is shipped with a an **installer** that can, in addition to installing the binary distribution, automatically download the latest versions of the plugins and samples from our repository.

.. warning:: The installer is currently only for Windows builds and Linux builds have to be set up manually. This includes unzipping the binary distribution and optionally downloading the latest plugin binaries and samples from the repository.

1. Download the latest stable or beta release from the `landing page <https://iolite-engine.com>`_ or, if you have subscribed to IOLITE PRO, from the `subscriber area <https://iolite-engine.com/subscribers>`_

2. Execute the installer, select the desired components and an installation directory, and execute IOLITE from the installation directory afterwards.

.. note:: If you encounter an issue when running IOLITE, please consult the section :ref:`faq`. If you are unable to solve the issue, please follow the steps depicted in :ref:`reporting_issues`.

Beta releases
-------------

Beta releases are released in between main releases for PRO subscribers. Please consider the following when working with a beta release of IOLITE:

.. important:: Betas are released more frequently and contain the latest features, but they can be a little less stable. Frequent changes to the API are expected.

1. Switch to the ``develop`` branch in our public GitHub repository and search for a tag matching the version of IOLITE you are using
2. Ensure that you are using a matching version of the open-source plugins. Due to potential API changes, there are separate GitHub releases available for the beta versions of IOLITE
  
Telemetry
---------

IOLITE, by default, gathers some elementary **anonymous usage statistics** to aid the development.

.. note:: This system will be extended in the future also to report crashes.

The following data is collected for each of the logged events:

UUID
   A unique identifier based on a **hash** of the addresses of **all** the network adapters installed in your system, e.g., ``8A36-E187-2C1F-51FB``. This hash **can not be** reversed to get access to the MAC addresses of your adapters
Platform
   Either ``Windows`` or ``Linux``
Build
  Either ``Free`` or ``Pro``
API version
  The version of the API, e.g., ``0.4.0``
Event name
  The event's name, e.g., ``startup``, ``shutdown``, etc.
Event metadata
  Currently unused and reserved for future use
  
You can turn off the telemetry system at any time by adding the following member to your ``app_metadata.json`` file:

.. code-block:: json
   
  "disable_telemetry": true