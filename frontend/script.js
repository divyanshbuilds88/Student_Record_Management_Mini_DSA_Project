// Frontend logic: talks to the C++ backend using fetch() and updates the page.

const API = "/api";
let editingRoll = null;        // null = adding a new student, number = editing that student
let topperRolls = [];          // roll numbers of the current topper(s)

// ---------- Page elements ----------
const form = document.getElementById("studentForm");
const rollInput = document.getElementById("rollNo");
const nameInput = document.getElementById("name");
const courseInput = document.getElementById("course");
const marksInput = document.getElementById("marks");
const formTitle = document.getElementById("formTitle");
const submitBtn = document.getElementById("submitBtn");
const cancelBtn = document.getElementById("cancelBtn");
const tableBody = document.getElementById("tableBody");
const emptyMessage = document.getElementById("emptyMessage");
const searchRoll = document.getElementById("searchRoll");
const toast = document.getElementById("toast");

// ---------- Helper: send a request to the backend ----------
async function request(url, options) {
  let response;
  try {
    response = await fetch(url, options);
  } catch (err) {
    throw new Error("Cannot reach the server. Is server.exe running?");
  }
  const data = await response.json().catch(() => ({}));
  if (!response.ok) {
    throw new Error(data.error || "Something went wrong.");
  }
  return data;
}

// ---------- Helper: show a short message ----------
let toastTimer;
function showToast(message, type) {
  toast.textContent = message;
  toast.className = "toast " + type;
  clearTimeout(toastTimer);
  toastTimer = setTimeout(() => toast.classList.add("hidden"), 3000);
}

// ---------- Show students in the table ----------
function renderTable(list) {
  tableBody.innerHTML = "";
  emptyMessage.classList.toggle("hidden", list.length > 0);

  list.forEach((student) => {
    const row = document.createElement("tr");

    // textContent is used so that typed text can never run as HTML
    const roll = document.createElement("td");
    roll.textContent = student.rollNo;

    const name = document.createElement("td");
    name.textContent = student.name;
    if (topperRolls.includes(student.rollNo)) {
      const badge = document.createElement("span");
      badge.className = "badge";
      badge.textContent = "TOPPER";
      name.appendChild(badge);
    }

    const course = document.createElement("td");
    course.textContent = student.course;

    const marks = document.createElement("td");
    marks.textContent = student.marks.toFixed(2);

    const actions = document.createElement("td");
    actions.className = "actions";

    const editBtn = document.createElement("button");
    editBtn.className = "btn small";
    editBtn.textContent = "Edit";
    editBtn.onclick = () => startEdit(student);

    const deleteBtn = document.createElement("button");
    deleteBtn.className = "btn small danger";
    deleteBtn.textContent = "Delete";
    deleteBtn.onclick = () => deleteStudent(student);

    actions.append(editBtn, deleteBtn);
    row.append(roll, name, course, marks, actions);
    tableBody.appendChild(row);
  });
}

// ---------- Load data from the backend ----------
async function loadStats() {
  const stats = await request(API + "/stats");
  document.getElementById("statCount").textContent = stats.count;
  document.getElementById("statAverage").textContent = stats.average.toFixed(2);
  topperRolls = stats.toppers.map((s) => s.rollNo);

  const topperText = document.getElementById("statTopper");
  if (stats.toppers.length === 0) {
    topperText.textContent = "-";
  } else {
    topperText.textContent =
      stats.toppers.map((s) => s.name).join(", ") + " (" + stats.toppers[0].marks.toFixed(2) + ")";
  }
}

async function loadStudents() {
  try {
    await loadStats();                              // stats first, so topper badges are correct
    const list = await request(API + "/students");
    renderTable(list);
  } catch (err) {
    showToast(err.message, "error");
  }
}

// ---------- Add / Update ----------
form.addEventListener("submit", async (event) => {
  event.preventDefault();
  const student = {
    rollNo: Number(rollInput.value),
    name: nameInput.value,
    course: courseInput.value,
    marks: Number(marksInput.value),
  };

  try {
    if (editingRoll === null) {
      await request(API + "/students", {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify(student),
      });
      showToast("Student added successfully!", "success");
    } else {
      await request(API + "/students/" + editingRoll, {
        method: "PUT",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify(student),
      });
      showToast("Student updated successfully!", "success");
    }
    resetForm();
    loadStudents();
  } catch (err) {
    showToast(err.message, "error");
  }
});

function startEdit(student) {
  editingRoll = student.rollNo;
  rollInput.value = student.rollNo;
  rollInput.disabled = true;                        // roll number cannot be changed
  nameInput.value = student.name;
  courseInput.value = student.course;
  marksInput.value = student.marks;
  formTitle.textContent = "Update Student";
  submitBtn.textContent = "Save Changes";
  cancelBtn.classList.remove("hidden");
  nameInput.focus();
}

function resetForm() {
  editingRoll = null;
  form.reset();
  rollInput.disabled = false;
  formTitle.textContent = "Add Student";
  submitBtn.textContent = "Add Student";
  cancelBtn.classList.add("hidden");
}
cancelBtn.addEventListener("click", resetForm);

// ---------- Delete ----------
async function deleteStudent(student) {
  if (!confirm("Delete " + student.name + " (Roll " + student.rollNo + ")?")) return;
  try {
    await request(API + "/students/" + student.rollNo, { method: "DELETE" });
    showToast("Student deleted successfully!", "success");
    if (editingRoll === student.rollNo) resetForm();
    loadStudents();
  } catch (err) {
    showToast(err.message, "error");
  }
}

// ---------- Search (backend uses linear search) ----------
document.getElementById("searchBtn").addEventListener("click", async () => {
  const roll = searchRoll.value.trim();
  if (roll === "") {
    showToast("Enter a roll number to search.", "error");
    return;
  }
  try {
    const student = await request(API + "/students/" + encodeURIComponent(roll));
    renderTable([student]);
  } catch (err) {
    showToast(err.message, "error");
  }
});

document.getElementById("showAllBtn").addEventListener("click", () => {
  searchRoll.value = "";
  loadStudents();
});

// ---------- Sort (backend uses bubble sort) ----------
document.getElementById("sortBtn").addEventListener("click", async () => {
  try {
    await request(API + "/sort", { method: "POST" });
    showToast("Sorted by marks (highest to lowest).", "success");
    loadStudents();
  } catch (err) {
    showToast(err.message, "error");
  }
});

// Load everything when the page opens
loadStudents();
