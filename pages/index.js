import Head from 'next/head'
import styles from '../styles/Home.module.css'

export default function Home() {
  return (
    <div className={styles.container}>
      <Head>
        <title>woof</title>
        <link rel="icon" href="/favicon.ico" />
      </Head>

      {[1,2,3,4,5].map(x => `number ${x} `)}
    </div>
  )
}
