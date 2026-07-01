---
pagetitle: Release Notes for JPEG Utilities Drivers
lang: en
header-includes: <link rel="icon" type="image/x-icon" href="_htmresc/favicon.png" />
---

::: {.row}
::: {.col-sm-12 .col-lg-4}

<center>
# Release Notes for
# <mark>STM32Cube JPEG Utilities Drivers</mark>
Copyright &copy; 2016-2023 STMicroelectronics\

[![ST logo](_htmresc/st_logo_2020.png)](https://www.st.com){.logo}
</center>

# Purpose

This driver provides a set of basic functions to convert JPEG "Minimum Coded Units" blocks (MCU) into/from standard RGB, Grayscale or CMYK pixels.
It can be used on top of the HAL JPEG driver. The following APIs are available:

-	JPEG_GetDecodeColorConvertFunc
-	JPEG_GetEncodeColorConvertFun


:::

::: {.col-sm-12 .col-lg-8}
# Update History

::: {.collapse}
<input type="checkbox" id="collapse-section5" checked aria-hidden="true">
<label for="collapse-section5" aria-hidden="true">V2.0.3 / 11-July-2023</label>
<div>

## Main Changes

### Component release

**Module changes**

-	[Licensing] Add license.* files and update license header

## Known Limitations

-	None

## Backward Compatibility

-	This version is backward compatible with previous version.

</div>
:::

::: {.collapse}
<input type="checkbox" id="collapse-section4" aria-hidden="true">
<label for="collapse-section4" aria-hidden="true">V2.0.2 / 18-May-2023</label>
<div>

## Main Changes

### Component release

**Module changes**

-	Fix English typos in comments

## Known Limitations

-	None

## Backward Compatibility

-	This version is backward compatible with previous version.

</div>
:::

::: {.collapse}
<input type="checkbox" id="collapse-section3" aria-hidden="true">
<label for="collapse-section3" aria-hidden="true">V2.0.1 / 19-April-2021</label>
<div>

## Main Changes

### Component release

**Module changes**

-	Fix English typos in comments

## Known Limitations

-	None

## Backward Compatibility

-	This version is backward compatible with previous version.

</div>
:::

::: {.collapse}
<input type="checkbox" id="collapse-section2" aria-hidden="true">
<label for="collapse-section2" aria-hidden="true">V2.0.0 / 03-June-2016</label>
<div>


## Main Changes

### Component release

**Module changes**

-	Add the support of JPEG preprocessing for color spaces: YCbCr, Gray scale and CMYK
-   The following updates made on the jpeg_utils_conf_template require update on the application code based on JPEG Utility drivers V1.0.0:
    - Add specific defines for color format support RGB565, RGB888, ARGB8888
    - Add specific define JPEG_SWAP_RB to manage RGB Red and Blue swap
    - Remove the following defines: JPEG_RED_OFFSET, JPEG_GREEN_OFFSET, JPEG_BLUE_OFFSET, JPEG_ALPHA_OFFSET and JPEG_BYTES_PER_PIXEL

## Known Limitations

-	None

## Backward Compatibility

-	This version breaks the compatibility with the previous version

</div>
:::

::: {.collapse}
<input type="checkbox" id="collapse-section1" aria-hidden="true">
<label for="collapse-section1" aria-hidden="true">V1.0.0 / 22-April-2016</label>
<div>


## Main Changes

### Component release

**Module changes**

-  First official release

</div>
:::

:::
:::

<footer class="sticky">
For complete documentation on STM32,visit: [[www.st.com/stm32](http://www.st.com)]

This release note uses up to date web standards and, for this reason, should not be opened with Internet Explorer
but preferably with popular browsers such as Google Chrome, Mozilla Firefox, Opera or Microsoft Edge.
</footer>
