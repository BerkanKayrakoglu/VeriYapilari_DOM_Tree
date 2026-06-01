// Yazar: Mustafa Ozturk
// Frontend - C API ile haberlesme ve agac gorsellestirme

const API_URL = 'http://127.0.0.1:8080/parse';

async function parse() {
    const textarea = document.getElementById('html-input');
    let html = textarea.value;
    
    const liveServerIndex = html.indexOf('<!-- Code injected by live-server -->');
    if (liveServerIndex !== -1) {
        html = html.substring(0, liveServerIndex).trim();
    }
    
    if (!html) {
        setStatus('Hata: HTML girisi bos!');
        return;
    }

    setStatus('Ayristiriliyor...');

    try {
        const response = await fetch(API_URL, {
            method: 'POST',
            headers: { 'Content-Type': 'text/plain' },
            body: html
        });

        if (!response.ok) throw new Error('Sunucu hatasi');

        const data = await response.json();
        renderTree(data);
        setStatus('Agac basariyla olusturuldu.');
    } catch (err) {
        setStatus('Hata: C sunucusuna baglanamadi. server.exe calisiyor mu?');
        console.error(err);
    }
}

function renderTree(node) {
    const container = document.getElementById('tree-container');
    container.innerHTML = '';
    container.appendChild(createNodeElement(node));
}

function createNodeElement(node) {
    const wrapper = document.createElement('div');
    wrapper.className = 'tree-node';

    const header = document.createElement('div');
    header.className = 'tree-node-header';
    header.dataset.id = node.id || '';
    header.dataset.class = node.class || '';

    const toggle = document.createElement('span');
    toggle.className = 'toggle';
    toggle.textContent = node.children && node.children.length > 0 ? '▼' : ' ';

    const tagSpan = document.createElement('span');
    tagSpan.className = 'tag-name';
    tagSpan.textContent = '<' + node.tag + '>';

    header.appendChild(toggle);
    header.appendChild(tagSpan);

    if (node.id) {
        const idSpan = document.createElement('span');
        idSpan.className = 'attr-id';
        idSpan.textContent = ' id="' + node.id + '"';
        header.appendChild(idSpan);
    }

    if (node.class) {
        const classSpan = document.createElement('span');
        classSpan.className = 'attr-class';
        classSpan.textContent = ' class="' + node.class + '"';
        header.appendChild(classSpan);
    }

    wrapper.appendChild(header);

    if (node.children && node.children.length > 0) {
        const childContainer = document.createElement('div');
        childContainer.className = 'tree-children';

        node.children.forEach(child => {
            childContainer.appendChild(createNodeElement(child));
        });

        wrapper.appendChild(childContainer);

        header.addEventListener('click', () => {
            const isOpen = childContainer.style.display !== 'none';
            childContainer.style.display = isOpen ? 'none' : 'block';
            toggle.textContent = isOpen ? '▶' : '▼';
        });
    }

    return wrapper;
}

function search() {
    clearHighlight();
    const query = document.getElementById('search-input').value.trim();
    if (!query) return;

    let type = null;
    let value = null;

    const idMatch = query.match(/id="([^"]+)"/);
    const classMatch = query.match(/class="([^"]+)"/);

    if (idMatch) {
        type = 'id';
        value = idMatch[1];
    } else if (classMatch) {
        type = 'class';
        value = classMatch[1];
    } else {
        setStatus('Format: id="deger" veya class="deger"');
        return;
    }

    const headers = document.querySelectorAll('.tree-node-header');
    let found = 0;

    headers.forEach(header => {
        const match = type === 'id'
            ? header.dataset.id === value
            : header.dataset.class === value;

        if (match) {
            header.classList.add('highlighted');
            header.scrollIntoView({ behavior: 'smooth', block: 'center' });
            found++;
        }
    });

    setStatus(found > 0 ? found + ' dugum bulundu.' : 'Dugum bulunamadi.');
}

function clearHighlight() {
    document.querySelectorAll('.highlighted').forEach(el => {
        el.classList.remove('highlighted');
    });
}

function setStatus(msg) {
    document.getElementById('status').textContent = msg;
}