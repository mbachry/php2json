import json
import pytest
import php2json


def decode(s):
    return json.loads(php2json.php2json(s).decode())


def test_simple_int():
    assert decode(b'i:157;') == 157


def test_int_bad_syntax():
    with pytest.raises(ValueError):
        decode(b'i:157')
    with pytest.raises(ValueError):
        decode(b'i:foo;')


def test_simple_string():
    assert decode(b's:3:"foo";') == 'foo'


def test_string_escaping():
    assert decode(b's:7:"\x08\x0c\n\r\t\\"";') == '\b\f\n\r\t\\\"'


def test_empty_string():
    assert decode(b's:0:"";') == ''


def test_bogus_length():
    with pytest.raises(ValueError):
        decode(b's:-1:"";')
    with pytest.raises(ValueError):
        decode(b's:foo:"";')
    with pytest.raises(ValueError):
        decode(b's::"";')


def test_string_bad_syntax():
    with pytest.raises(ValueError):
        decode(b's:3:"foo"')
    with pytest.raises(ValueError):
        decode(b's:3:foo";')
    with pytest.raises(ValueError):
        decode(b's:3:"foo;')


# def test_string_unicode():
#     ...


def test_null():
    assert decode(b'N;') is None


def test_null_bad_syntax():
    with pytest.raises(ValueError):
        decode(b'N')


def test_bool():
    assert decode(b'b:0;') is False
    assert decode(b'b:1;') is True


def test_bool_bad_syntax():
    with pytest.raises(ValueError):
        decode(b'b:1')
    with pytest.raises(ValueError):
        decode(b'b:foo;')
    with pytest.raises(ValueError):
        decode(b'b:2;')


def test_array():
    assert decode(b'a:4:{i:0;i:10;i:1;s:1:"a";i:2;N;i:3;b:1;}') == {'0': 10, '1': 'a', '2': None, '3': True}


def test_nested_array():
    assert decode(b'a:2:{i:1;a:1:{i:1;i:1;}i:2;a:1:{i:2;i:2;}}') == {'1': {'1': 1}, '2': {'2': 2}}


def test_empty_array():
    assert decode(b'a:0:{}') == {}


def test_bogus_array_length():
    with pytest.raises(ValueError):
        decode(b'a:-1:{}')
    with pytest.raises(ValueError):
        decode(b'a:foo:{}')
    with pytest.raises(ValueError):
        decode(b'a::{}')


def test_array_as_key_not_allowed():
    with pytest.raises(ValueError):
        decode(b'a:1:{a:1:{i:0;i:1;}i:1;}')


def test_array_bad_syntax():
    with pytest.raises(ValueError):
        decode(b'a:1:{i:0;i:1}')
    with pytest.raises(ValueError):
        decode(b'a:1:{i:0;i:1;')
    with pytest.raises(ValueError):
        decode(b'a:1:i:0;i:1;}')
    with pytest.raises(ValueError):
        decode(b'a:1:{i:0i:1;}')
