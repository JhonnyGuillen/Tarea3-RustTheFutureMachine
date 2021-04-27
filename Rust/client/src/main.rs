use std::io::{self, ErrorKind, Read, Write};
use std::net::TcpStream;
use std::sync::mpsc::{self, TryRecvError};
use std::thread;
use std::time::Duration;
use std::{fs::File};

const LOCAL: &str = "127.0.0.1:6000";
const MSG_SIZE: usize = 32;

fn main() {
    let mut client = TcpStream::connect(LOCAL).expect("Fallo de conectar el cliente");
    client.set_nonblocking(true).expect("Falla al intentar inicializar el non-blocking");

    let (tx, rx) = mpsc::channel::<String>();

    thread::spawn(move || loop {
        let mut buff = vec![0; MSG_SIZE];
        match client.read_exact(&mut buff) {
            Ok(_) => {
                let msg = buff.into_iter().take_while(|&x| x != 0).collect::<Vec<_>>();
                println!("Mensaje recivido: {:?}", msg);
                let asd = String::from_utf8(msg).expect("Mensaje utf8 invalido");
                println!("Server recivido: {:?}", asd);
                let mut mi_archivo = File::create("archivo.txt").expect("No se pudo crear el arcivo");
                mi_archivo.write_all(asd.as_bytes()).expect("Error al guardar el archivo");
            },
            Err(ref err) if err.kind() == ErrorKind::WouldBlock => (),
            Err(_) => {
                println!("Coneccion con elserver exitoso");
                break;
            }
        }

        match rx.try_recv() {
            Ok(msg) => {
                let mut buff = msg.clone().into_bytes();
                buff.resize(MSG_SIZE, 0);
                client.write_all(&buff).expect("Fallo del socket al escribir");
                println!("Mensaje enviado {:?}", msg);
            }, 
            Err(TryRecvError::Empty) => (),
            Err(TryRecvError::Disconnected) => break
        }

        thread::sleep(Duration::from_millis(100));
    });

    println!("Escriba el mensaje:");
    loop {
        let mut buff = String::new();
        io::stdin().read_line(&mut buff).expect("Lectura de stdin fallida");
        let msg = buff.trim().to_string();
        if msg == ":quit" || tx.send(msg).is_err() {break}
    }
    println!("bye bye!");

}