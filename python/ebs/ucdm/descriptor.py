

from ebs.modbus.client import ModbusServerException
from ebs.modbus.device import ModbusDevice
from pymodbus.mei_message import ReadDeviceInformationRequest


class _ModbusReadDeviceInfoDummyResponse(object):
    more_follows = 0xFF
    next_object_id = 0


def _parse_string(value):
    return value.decode('utf-8').strip().rstrip('\x00')


def _parse_strings(value):
    return [_parse_string(x) for x in value]


def _parse_hexbytes(value):
    return ''.join(["%02X" % x for x in bytearray(value)])


class ModbusDescriptorMixin(object):
    _descriptor_read_code = 0x03

    _descriptor_tags = {
        0x00: ('vendor_name', _parse_string),
        0x01: ('hardware_revision', _parse_string),
        0x02: ('firmware_revision', _parse_string),
        0x03: ('vendor_url', _parse_string),
        0x04: ('hardware_name', _parse_string),
        0x05: ('hardware_variant', _parse_string),
        0x06: ('firmware_name', _parse_string),
        0x80: ('serial_number', _parse_hexbytes),
        0x81: ('lib_versions', _parse_strings),
    }

    def __init__(self, *args, **kwargs):
        self._descriptors = {}
        super(ModbusDescriptorMixin, self).__init__(*args, **kwargs)
        if not hasattr(self, '_registry'):
            self._registry = {}
        if 'descriptor_tags' not in self._registry.keys():
            self._registry['descriptor_tags'] = {}
        self._registry['descriptor_tags'].update(self._descriptor_tags)

    @property
    def descriptors(self):
        if not self._descriptors:
            self._descriptors = self._get_parsed_descriptors()
        return self._descriptors

    def read_descriptors(self):
        self._descriptors = self._get_parsed_descriptors()

    def _get_parsed_descriptors(self):
        descriptors = self._get_descriptors()
        tags = list(descriptors.keys())
        for tag in tags:
            value = descriptors.pop(tag)

            parser = None
            if tag in self._registry['descriptor_tags'].keys():
                parser = self._registry['descriptor_tags'][tag]

            if parser:
                descriptors[parser[0]] = parser[1](value)
            else:
                descriptors[tag] = _parse_string(value)
        return descriptors

    def _get_descriptors(
            self  # type: ModbusDevice
    ):
        rr = _ModbusReadDeviceInfoDummyResponse()
        rval = {}
        while rr.more_follows:
            rq = ReadDeviceInformationRequest(
                unit=self.address, read_code=self._descriptor_read_code,
                object_id=rr.next_object_id
            )
            try:
                rr = self.mc.execute(rq)
                rval.update(rr.information)
            except ModbusServerException:
                pass
        return rval

    def print_descriptors(self):
        for tag in sorted(self.descriptors.keys(), key=lambda x: str(x)):
            print("{0:>20} : {1}".format(tag, self.descriptors[tag]))

    def __getattr__(self, item):
        if item in self._descriptors.keys():
            return self._descriptors[item]
        return super(ModbusDescriptorMixin, self).__getattr__(item)
