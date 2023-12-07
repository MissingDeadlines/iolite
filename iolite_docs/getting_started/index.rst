👀 Getting Started
===================

This section contains installation instructions and guides to get you started.

Installing portable builds (Windows and Linux)
----------------------------------------------

Installing IOLITE is trivial. Simply download a release from the `landing page <https://iolite-engine.com>`_ or, if you have subscribed to IOLITE PRO, from the `subscriber area <https://iolite-engine.com/subscribers>`_ and unzip it. Finally, launch the IOLITE executable. That's it. 🙂 

If you encounter an issue when running IOLITE, please consult the section :ref:`faq`. If you are unable to solve the issue, please follow the steps depicted in :ref:`reporting_issues`.

Beta releases (PRO only)
------------------------

Beta releases are released in between main releases for PRO subscribers. Please consider the following when working with a beta release of IOLITE:

.. important:: Beta releases are released more frequently and contain the latest features, but they can be a little less stable.

- Please switch to the ``develop`` branch in our public GitHub repository and search for a tag matching the version of IOLITE you are using
- Please ensure that you are using a matching version of the open-source plugins. Due to potential API changes, there are separate plugin releases available for the beta versions of IOLITE
  
Telemetry (Usage statistics)
----------------------------

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


.. toctree::
  :hidden:
  
  guides