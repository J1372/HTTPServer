

fetch('/hello', { method: 'POST', body: "Hello" })
.then(res => {
    console.log(res.status);
    return res.text()
})
.then(console.log)
.then(_ => fetch('/hello_delayed', { method: 'POST', body: "Hello" }))
.then(res => {
    console.log(res.status);
    return res.text()
})
.then(console.log)
