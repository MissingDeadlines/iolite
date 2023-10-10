.. _faq:

⁉️ Frequently asked questions
=============================

Here's a collection of frequently asked questions. Make sure to go through each one if you're encountering an issue. Chances are high that it is already known and documented here.

.. _Discord community: https://discord.com/invite/SZjfhw7z75

.. note:: If you do not find the answer you are looking for, head to our `Discord community`_ and raise the question there.
   
What can I do with IOLITE? What is its license? Is it open-source software?
---------------------------------------------------------------------------
   
IOLITE is available as a free and paid (PRO) version. You're **free to create either commercial or non-commercial software with both versions**. While IOLITE's engine core is considered proprietary software, some of its functionality is provided as :ref:`open-source plugins <plugins>` licensed using the MIT license. The paid version finances the project's development and adds some additional features, as listed in :ref:`pro features`.

Both the PRO and the free versions are shipped without any form of DRM and share a straightforward license agreement:
   
.. code-block:: text
   
  IOLITE Software License Agreement
  =================================

  This license has a single goal: letting you create incredible things with IOLITE without worrying about strict licensing rules.

  Just remember that your support enables me to work on IOLITE for the long haul.

  TL;DR
  -----

  Feel free to utilize IOLITE as desired, provided that you include all licenses and acknowledge that no warranty is implied; use IOLITE at your own risk.

  Full License
  ------------

  Copyright 2023 Missing Deadlines (Benjamin Wrensch)

    1. You're allowed to use IOLITE for as many projects as you like (commercial or non-commercial).
    2. This license and all other licenses included in the IOLITE software distribution have to be shipped with every project IOLITE is used for.
    3. THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
       INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
       IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
       WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
       OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

  Please do not download or install IOLITE if you do not agree to all of these terms. 

  If this license does not fit your needs, feel free to contact us any time; I'm sure we'll be able to work out a custom solution.

What are the overall system requirements?
-----------------------------------------

**General Requirements**

- **System:** Windows 10+ or Linux
- **GPU:** Support for Vulkan 1.2 or higher
- **CPU:** Support for the AVX2 instruction set

**Optimal Performance**

- **GPU:** NVIDIA GTX 1080 (or equivalent) and higher

.. warning:: Please note that integrated graphics cards are not supported. Even if IOLITE starts, chances are high that the performance is far from optimal.
   
Is IOLITE considered a finished product?
----------------------------------------
   
IOLITE is currently in its initial development phase and evolves constantly. The first *official release* will be marked by version 1.0.

.. note:: While creating amazing things with IOLITE is possible, we can not guarantee feature and API stability at this point. Please remember this when deciding to work on a project using IOLITE.

Help, IOLITE crashes during startup!
------------------------------------

Many checks are in place to ensure you end up with a meaningful error message in any possible failure case. If you encounter a startup crash, the chances are high that your GPU driver is outdated. Please use the most recent drivers from NVIDIA or AMD:

**Drivers for NVIDIA GPUs**
   https://www.nvidia.com/download/index.aspx
**Drivers for AMD GPUs**
   https://www.amd.com/en/support

Using the latest proprietary driver is your best bet when using Linux. Please ensure that your GPU driver is working correctly before trying to launch IOLITE.

If you're experiencing issues with the standalone Linux builds, it's potentially due to mismatches between the runtime libraries utilized by IOLITE and your Linux distribution. In this case, please use a build from our Flatpak repository, which ships with the correct versions of all the utilized runtime libraries. You can find details on how to set up the Flatpak builds at the top of this page.

How the hell do I exit the game mode again!?
----------------------------------------------

The default keybinding for opening the editor is ``[F3]``. You can adjust this in the ``keybindings_global.json`` file.

I only see a simple default scene; where are the samples?
-----------------------------------------------------------

Head over to our `GitHub repository <https://github.com/MissingDeadlines/iolite>`_ and place the contents of the ``iolite_samples`` directory right next to the IOLITE executable, overwriting any existing files.

I've installed the latest build via Flatpak but can't find where IOLITE stores its data!
------------------------------------------------------------------------------------------

When launching IOLITE for the first time, it installs all its data in a directory named according to the current version in the following directory:

.. code-block:: console

  /.var/app/com.missing_deadlines.IOLITE/

Deleting this directory can be used to reset IOLITE to its defaults.

.. Is it possible to use Lua auto-completion with IOLITE?
  --------------------------------------------------------

  You can find IOLITE's Lua scripting API as a LUA file in the Lua API documentation. Drop it close to your script, and your favorite IDE should pick it up automatically.

  If you use Visual Studio Code, install the `Lua extension <https://marketplace.visualstudio.com/items?itemName=sumneko.lua>`_ on the marketplace.

I would love to try writing a native plugin using the C/C++ API. Do I need IOLITE PRO?
----------------------------------------------------------------------------------------

The API header file is publicly available via our `public GitHub repository <https://github.com/MissingDeadlines/iolite>`_. In addition, the free version can also load native plugins, so it's possible to evaluate this feature before deciding to subscribe. Loading non-factory plugins in the free version will trigger a watermark.

.. _reporting_issues:

I've encountered an issue that is not referenced in this FAQ. What should I do?
-------------------------------------------------------------------------------

.. _issue tracker: https://github.com/MissingDeadlines/iolite/issues

Bugs and feature requests can be submitted via our `issue tracker`_. You should also consider joining our `Discord community`_ and raising it there. See you there! 👋