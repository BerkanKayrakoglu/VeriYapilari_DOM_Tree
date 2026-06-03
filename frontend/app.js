
// Frontend - C API ile haberlesme ve agac gorsellestirme

const API_URL    = 'http://localhost:8080/parse';
const SEARCH_URL = 'http://localhost:8080/search';
const ANALYZE_URL= 'http://localhost:8080/analyze';
const AI_URL     = 'http://localhost:5001/generate';

function getCleanHTML() {
    const textarea = document.getElementById('html-input');
    let html = textarea.value;
    const liveIdx = html.indexOf('<!-- Code injected by live-server -->');
    if (liveIdx !== -1) html = html.substring(0, liveIdx).trim();
    return html;
}

async function parse() {
    const html = getCleanHTML();
    if (!html) { setStatus('Hata: HTML girisi bos!'); return; }

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
        analyze(html);
    } catch (err) {
        setStatus('Hata: C sunucusuna baglanamadi. server.exe calisiyor mu?');
        console.error(err);
    }
}

async function analyze(html) {
    try {
        const response = await fetch(ANALYZE_URL, {
            method: 'POST',
            headers: { 'Content-Type': 'text/plain' },
            body: html
        });
        if (!response.ok) return;
        const data = await response.json();
        showStats(data);
    } catch (err) {
        // sessizce hata yutulur
    }
}

function showStats(data) {
    const panel = document.getElementById('stats-panel');
    if (!panel) return;
    document.getElementById('stat-nodes').textContent = 'Toplam Dugum: ' + data.total_nodes;
    document.getElementById('stat-depth').textContent = 'Derinlik: ' + data.depth;
    document.getElementById('stat-bfs').textContent   = 'BFS: ' + data.bfs_order.join(' → ');
    document.getElementById('stat-dfs').textContent   = 'DFS: ' + data.dfs_order.join(' → ');
    panel.style.display = 'flex';
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
    header.dataset.id    = node.id    || '';
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

async function search() {
    clearHighlight();
    const query = document.getElementById('search-input').value.trim();
    if (!query) return;

    let type = null, value = null;
    const idMatch    = query.match(/id="([^"]+)"/);
    const classMatch = query.match(/class="([^"]+)"/);

    if (idMatch)         { type = 'id';    value = idMatch[1]; }
    else if (classMatch) { type = 'class'; value = classMatch[1]; }
    else { setStatus('Format: id="deger" veya class="deger"'); return; }

    const html = getCleanHTML();
    if (!html) { setStatus('Once agac olusturun.'); return; }

    try {
        const response = await fetch(SEARCH_URL, {
            method: 'POST',
            headers: { 'Content-Type': 'text/plain' },
            body: type + ':' + value + '\n' + html
        });
        const nodes = await response.json();

        const headers = document.querySelectorAll('.tree-node-header');
        let found = 0;
        headers.forEach(header => {
            const match = type === 'id'
                ? header.dataset.id    === value
                : header.dataset.class === value;
            if (match) {
                header.classList.add('highlighted');
                if (found === 0) header.scrollIntoView({ behavior: 'smooth', block: 'center' });
                found++;
            }
        });

        const algo = type === 'id' ? 'Hash Table O(1)' : 'BFS O(N)';
        setStatus(nodes.length + ' dugum bulundu. [' + algo + ']');
    } catch (err) {
        setStatus('Arama hatasi: sunucuya baglanamadi.');
        console.error(err);
    }
}

async function generateHTML() {
    setStatus('Yapay zeka veri uretiliyor...');
    try {
        const response = await fetch(AI_URL + '?depth=4&width=3');
        if (!response.ok) throw new Error();
        const data = await response.json();
        document.getElementById('html-input').value = data.html;
        setStatus('Yapay zeka verisi yuklendi. Agaci olusturun.');
    } catch (err) {
        setStatus('AI servisi calisiyor mu? (python ai_service/app.py)');
    }
}

function clearHighlight() {
    document.querySelectorAll('.highlighted').forEach(el => {
        el.classList.remove('highlighted');
    });
}

function setStatus(msg) {
    document.getElementById('status').textContent = msg;
}
