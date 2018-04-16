

from ebs.modbus.client import ModbusServerException
from ebs.modbus.device import ModbusDevice
from pymodbus.mei_message import ReadDeviceInformationRequest


class _ModbusReadDeviceInfoDummyResponse(object):
    more_follows = 0xFF
    next_object_id = 0


class ModbusDescriptorMixin(object):
    _descriptor_read_code = 0x03

    def __init__(self, *args, **kwargs):
        self._descriptors = {}
        super(ModbusDescriptorMixin, self).__init__(*args, **kwargs)

    @property
    def descriptors(self):
        if not self._descriptors:
            self._descriptors = self._descriptors_get_parse()
        return self._descriptors

    def descriptors_refresh(self):
        self._descriptors_get_parse()

    def _descriptors_get_parse(self):
        descriptors = self._descriptors_get()
        return descriptors

    def _descriptors_get(
            self  # type: ModbusDevice
    ):
        rr = _ModbusReadDeviceInfoDummyResponse()
        rval = {}
        while rr.more_follows:
            rq = ReadDeviceInformationRequest(unit=self.address,
                                              read_code=self._descriptor_read_code,
                                              object_id=rr.next_object_id)
            rr = self.mc.execute(rq)
            rval.update(rr.information)
        return rval
