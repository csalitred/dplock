import logging
from typing import Callable

import pytest
from pytest_embedded_idf.dut import IdfDut
from pytest_embedded_qemu.app import QemuApp
from pytest_embedded_qemu.dut import QemuDut

@pytest.mark.esp32s3
