// tạo biến cho loading dialog 
const loadingDialog = document.getElementById('loadingDialog');
function showLoadingDialog() {
    loadingDialog.style.display = 'block';
}
function hideLoadingDialog() {
    loadingDialog.style.display = 'none';
}
//tạo biến dialog thêm nhân viên
const dialogthemvantay = document.getElementById('dialogthemvantay');
function showdialogvantay() {
    dialogthemvantay.style.display = 'block';
}

function hidendialogvantay() {
    dialogthemvantay.style.display = 'none';
}

database.ref("dsnhanvien").on("value", (snapshot) => {
    snapshot.forEach((childSnapshot) => {
        const employee = childSnapshot.val();
        const today = new Date();
        const dateTime = `${today.getDate()}-${today.getMonth() + 1}-${today.getFullYear()}`;
        const hoursTime = today.getHours();
        const minutesTime = today.getMinutes();
        //xac dinh xem nhan vien nao dc cham cong
        const id = employee.statuschamcong;

        if (employee.statuschamcong != 0) {
            const chamcongRef = database.ref(`dsnhanvien/${id}/chamcong/${dateTime}`);
            chamcongRef.once("value", (snapshot1) => {
                if (!snapshot1.exists()) {
                    chamcongRef.update({
                        giovao: hoursTime,
                        phutvao: minutesTime,
                    }).then(() => {
                        database.ref(`dsnhanvien/${id}`).update({
                            statuschamcong: 0
                        });
                    });

                }
                else {
                    database.ref(`dsnhanvien/${id}/chamcong/${dateTime}`).once("value", (snapshot2) => {
                        const checktontai = snapshot2.val();
                        console.log(checktontai.giovao)
                        if (hoursTime - checktontai.giovao > 0) {
                            chamcongRef.update({
                                giora: hoursTime,
                                phutra: minutesTime,
                                giovao: checktontai.giovao,
                                phutvao: checktontai.phutvao,
                            }).then(() => {
                                database.ref(`dsnhanvien/${id}`).update({
                                    statuschamcong: 0
                                });
                            });
                        } else {
                            console.log("Chưa làm được 1 tiếng không chấm công!");
                            setTimeout(() => {
                                database.ref(`dsnhanvien/${id}`).update({
                                    statuschamcong: 0
                                });
                            }, 1500);

                        }

                    });
                }
            });

        };
    });
});


// Lấy giá trị từ Local
document.addEventListener('DOMContentLoaded', function () {
    const loginStatus = localStorage.getItem('login');
    if (loginStatus === '0') {
        window.location.href = 'pages-login.html'
    }
});
function logout() {
    localStorage.setItem('login', '0');
    window.location.href = 'pages-login.html'
}



