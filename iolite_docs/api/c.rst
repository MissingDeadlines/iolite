.. _api_c:

Native C API
============

This section shows the API header file and serves as a reference for IOLITE's C API. The header file is dependency-free, kept as minimal as possible, and can be compiled with all common C or C++ compilers.

All functions, data structures, and interfaces are documented in the header file itself.

For detailed instructions on writing plugins and the general usage of the API, please header over to the :ref:`writing plugins` section to get started.

.. note:: 
  All versions of IOLITE's C API header file are available `in our public GitHub repository <https://github.com/MissingDeadlines/iolite/blob/main/iolite_c_api/iolite_api.h>`_. 
.. warning::
  Please make sure to use a header file that matches the binary release of IOLITE you are using for your project. All IOLITE binary releases come with an according tag in the repository.

iolite_api.h
------------
   
.. literalinclude:: ../../iolite_c_api/iolite_api.h
  :language: c