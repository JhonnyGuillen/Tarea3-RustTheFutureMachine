use std::io::{ErrorKind, Read, Write};
use std::net::TcpListener;
use std::sync::mpsc;
use std::thread;
use std::fs;

const LOCAL: &str = "127.0.0.1:6000";
const MSG_SIZE: usize = 32;
const FILE_SIZE: usize = 1000;

fn sleep() {
    thread::sleep(::std::time::Duration::from_millis(100));
}

fn main() {
    let server = TcpListener::bind(LOCAL).expect("Listener failed to bind");
    server.set_nonblocking(true).expect("failed to initialize non-blocking");

    let mut clients = vec![];
    let (tx, rx) = mpsc::channel::<String>();
    loop {
        if let Ok((mut socket, addr)) = server.accept() {
            println!("Cliente {} conectado", addr);

            let tx = tx.clone();
            clients.push(socket.try_clone().expect("Fallo al clonar el cliente"));

            thread::spawn(move || loop {
                let mut buff = vec![0; MSG_SIZE];

                match socket.read_exact(&mut buff) {
                    Ok(_) => {
                        let msg = buff.into_iter().take_while(|&x| x != 0).collect::<Vec<_>>();
                        let msg = String::from_utf8(msg).expect("Mensaje utf8 invalido");

                        println!("{}: {:?}", addr, msg);
                        let contents = fs::read_to_string(msg.to_string()).expect("Algo ha fallado al leer el archivo.");
                        println!("Contenido:\n{}", contents);

                        tx.send(msg).expect("Fallo al enviar msg a rx");

                        tx.send(contents).expect("Archivo no encontrado");
                    }, 
                    Err(ref err) if err.kind() == ErrorKind::WouldBlock => (),
                    Err(_) => {
                        println!("Cerrando coneccion con: {}", addr);
                        break;
                    }
                }

                sleep();
            });
        }

        if let Ok(msg) = rx.try_recv() {
            clients = clients.into_iter().filter_map(|mut client| {
                let mut buff = msg.clone().into_bytes();
                buff.resize(MSG_SIZE, 0);

                client.write_all(&buff).map(|_| client).ok()
            }).collect::<Vec<_>>();
        }

        sleep();
    }
}
