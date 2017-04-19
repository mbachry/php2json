import timeit
import json
import phpserialize
import php2json
import ujson


d = {}
for i in range(400):
    d[str(i)] = {'1': 'foo', 'bar': '2', 'baz': None, 'boo': True}
data = phpserialize.dumps(d)


res1 = timeit.timeit('phpserialize.loads(data)', number=400, globals=locals())
res2 = timeit.timeit('ujson.loads(php2json.php2json(data))', number=400, globals=locals())
res3 = timeit.timeit('json.loads(php2json.php2json(data))', number=400, globals=locals())

print('phpserialize: {:.2f}s'.format(res1))
print('php2json+ujson: {:.2f}s'.format(res2))
print('php2json+json: {:.2f}s'.format(res3))
print('speedup (ujson): {}%'.format(int(res1 / res2 * 100)))
print('speedup (json): {}%'.format(int(res1 / res3 * 100)))
